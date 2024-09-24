import sys

from PyQt5.Qt import pyqtSignal, QApplication, Qt, pyqtSlot, QObject
from PyQt5.QtGui import QPainter, QPainterPath, QColor, QPen, QResizeEvent, QWindow
from PyQt5.QtWidgets import QFrame, QGridLayout, QPushButton, QLineEdit, QTableWidget, QScrollBar, QHBoxLayout, QLabel, QDialog, QDialogButtonBox, QMessageBox
from babel.messages.jslexer import name_re


class AddParamDialog(QDialog):
    __slots__ = ("__input", "__check_is_busy")

    add_param = pyqtSignal(str)

    def __init__(self, parent, check_is_busy):
        super().__init__(parent)

        layout = QGridLayout()
        self.setLayout(layout)
        r1 = QHBoxLayout()
        r2 = QHBoxLayout()
        layout.addLayout(r1, 1, 0)
        layout.addLayout(r2, 2, 0)

        title = QLabel("Add param", self)
        layout.addWidget(title, 0, 0, Qt.AlignHCenter)

        name_title = QLabel("Name:", self)
        r1.addWidget(name_title, 0, Qt.AlignRight)

        name_input = QLineEdit(self)
        r1.addWidget(name_input, 1)

        cancel_btn = QPushButton("Cancel")
        r2.addWidget(cancel_btn, 0)

        r2.addStretch(1)

        ok_btn = QPushButton("Add param")
        r2.addWidget(ok_btn, 0)

        layout.setRowStretch(3, 1)

        self.__input = name_input
        cancel_btn.clicked.connect(self.__cancel)
        ok_btn.clicked.connect(self.__ok)
        self.__check_is_busy = check_is_busy

    @pyqtSlot()
    def __cancel(self):
        self.destroy()

    @pyqtSlot()
    def __ok(self):
        name = self.__input.text()
        if name == "x" or not name.isidentifier():
            QMessageBox.about(self, "Wrong name", "Provided name can't be used correctly")
            return
        if self.__check_is_busy(name):
            QMessageBox.about(self, "Name duplication", "Provided name already defined for another parameter")
            return

        self.add_param.emit(name)
        self.destroy()


class Params(QFrame):
    __slots__ = ("__row_descriptors", "__table", "__used_names")

    remove_param = pyqtSignal(str)
    set_param = pyqtSignal(str, float)
    __add_param = pyqtSignal(str)

    def __init__(self, parent):
        super(Params, self).__init__(parent)

        layout = QGridLayout()
        self.setLayout(layout)

        table = QTableWidget(self)
        layout.addWidget(table, 1, 0)

        scroll = QScrollBar(self)
        layout.addWidget(scroll, 1, 1)
        table.addScrollBarWidget(scroll, Qt.AlignRight)
        table.horizontalHeader().hide()
        table.horizontalHeader().setStretchLastSection(True)

        add = QPushButton("Add param", self)
        layout.addWidget(add, 0, 0, 1, 2)
        add.clicked.connect(self.__creation_dialog)

        self.__table = table
        self.__row_descriptors = []
        self.__used_names = set()

        table.insertColumn(0)
        table.insertColumn(1)
        table.insertColumn(2)

    @pyqtSlot()
    def __creation_dialog(self):
        b = AddParamDialog(self, self.__used_names.__contains__)
        b.add_param.connect(self.__add)
        b.show()

    @pyqtSlot(str)
    def __add(self, name):
        self.__table.insertRow(len(self.__row_descriptors))
        d = Params.__RowDescriptor(self.__table, len(self.__row_descriptors), name)
        d.delete.connect(self.__delete)
        d.change.connect(self.__change)
        self.__row_descriptors.append(d)
        self.__used_names.add(name)

    @pyqtSlot(int, str)
    def __delete(self, ordinal, name):
        self.__row_descriptors.pop(ordinal)
        for d in self.__row_descriptors[ordinal:]:
            d.ordinal -= 1
        self.__table.removeRow(ordinal)
        self.remove_param.emit(name)
        self.__used_names.discard(name)

    @pyqtSlot(str, str)
    def __change(self, name, raw):
        try:
            value = float(raw)
        except ValueError:
            self.remove_param.emit(name)
            return

        self.set_param.emit(name, value)

    class __RowDescriptor(QObject):
        __slots__ = ("ordinal", "__name", "__value", "__delete" ,"__name_w")

        delete = pyqtSignal(int, str)
        change = pyqtSignal( str, str)

        def __init__(self, parent: QTableWidget, ordinal, name):
            super().__init__(parent)
            self.ordinal = ordinal
            self.__old_name = ""

            self.__delete = QPushButton("x", parent)
            parent.setCellWidget(ordinal, 0, self.__delete)
            self.__delete.clicked.connect(self.__on_delete)

            self.__name = name
            self.__name_w = QLabel(name, parent)
            parent.setCellWidget(ordinal, 1,  self.__name_w)

            self.__value = QLineEdit(parent)
            parent.setCellWidget(ordinal, 2, self.__value)
            self.__value.textChanged.connect(self.__on_value_change)

        @pyqtSlot()
        def __on_delete(self):
            self.delete.emit(self.ordinal, self.__name)
            self.__delete.destroy()
            self.__name_w.destroy()
            self.__value.destroy()

        @pyqtSlot()
        def __on_value_change(self):
            self.change.emit(self.__name, self.__value.text())


class MainWindow(QFrame):
    __slots__ = ("__path",)

    scroll_x = pyqtSignal(int)
    scroll_y = pyqtSignal(int)
    zoom_x = pyqtSignal(int)
    zoom_y = pyqtSignal(int)
    raw_expression_changed = pyqtSignal(str)
    set_param = pyqtSignal(str, float)
    remove_param = pyqtSignal(str)
    set_line = pyqtSignal(object)
    resized = pyqtSignal(int, int)

    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)

        layout = QGridLayout()
        self.setLayout(layout)

        for t, x, y, cb in (
                ("-", 0, 2, self.__zoom_x_dec),
                ("+", 4, 2, self.__zoom_x_inc),
                ("-", 2, 4, self.__zoom_y_dec),
                ("+", 2, 0, self.__zoom_y_inc),

                ("<", 1, 2, self.__scroll_x_dec),
                (">", 3, 2, self.__scroll_x_inc),
                ("^", 2, 1, self.__scroll_y_inc),
                ("v", 2, 3, self.__scroll_y_dec),
        ):
            btn = QPushButton(t, self)
            btn.setFixedWidth(50)
            btn.setFixedHeight(50)
            layout.addWidget(btn, y, x)
            btn.clicked.connect(cb)

        self.__input = QLineEdit(self)
        layout.addWidget(self.__input, 0, 4, 1, 3)
        self.__input.textChanged.connect(self.__on_expr_change)

        layout.setColumnStretch(5, 1)
        layout.setColumnStretch(6, 1)
        layout.setRowStretch(5, 1)
        layout.setRowStretch(6, 2)

        params = Params(self)
        layout.addWidget(params, 1, 6, 5, 1)
        params.set_param.connect(self.__set_param)
        params.remove_param.connect(self.__remove_param)

        self.set_line.connect(self.__set_path)
        self.__path = None

    @pyqtSlot(str, float)
    def __set_param(self, name, value):
        self.set_param.emit(name, value)

    @pyqtSlot(str)
    def __remove_param(self, name):
        self.remove_param.emit(name)

    @pyqtSlot()
    def __on_expr_change(self):
        self.raw_expression_changed.emit(self.__input.text())

    @pyqtSlot(object)
    def __set_path(self, path):
        self.__path = path
        self.repaint()

    def paintEvent(self, a0):
        w = self.width()
        h = self.height()
        painter = QPainter()
        painter.begin(self)
        painter.setPen(QPen(QColor(0, 0, 0), 3))
        painter.fillRect(0, 0, w, h, QColor(255, 255, 255))
        if self.__path is not None:
            painter.drawPath(self.__path.build(w, h))
        painter.end()

    @pyqtSlot()
    def __scroll_x_inc(self):
        self.scroll_x.emit(+1)

    @pyqtSlot()
    def __scroll_x_dec(self):
        self.scroll_x.emit(-1)

    @pyqtSlot()
    def __scroll_y_inc(self):
        self.scroll_y.emit(+1)

    @pyqtSlot()
    def __scroll_y_dec(self):
        self.scroll_y.emit(-1)

    @pyqtSlot()
    def __zoom_x_inc(self):
        self.zoom_x.emit(+1)

    @pyqtSlot()
    def __zoom_x_dec(self):
        self.zoom_x.emit(-1)

    @pyqtSlot()
    def __zoom_y_inc(self):
        self.zoom_y.emit(+1)

    @pyqtSlot()
    def __zoom_y_dec(self):
        self.zoom_y.emit(-1)

    def resizeEvent(self, a0: QResizeEvent):
        self.resized.emit(a0.size().width(), a0.size().height())


class ExchangeQtWrapper(QObject):
    __slots__ = ("__exchange",)

    set_line = pyqtSignal(object)

    def __init__(self, exchange):
        super().__init__()
        self.__exchange = exchange

    @pyqtSlot(int)
    def scroll_x(self, steps):
        self.__exchange.scroll_x(steps)

    @pyqtSlot(int)
    def scroll_y(self, steps):
        self.__exchange.scroll_y(steps)

    @pyqtSlot(int)
    def zoom_x(self, steps):
        self.__exchange.zoom_x(steps)

    @pyqtSlot(int)
    def zoom_y(self, steps):
        self.__exchange.zoom_y(steps)

    @pyqtSlot(str, float)
    def add_param(self, name, value):
        self.__exchange.add_param(name, value)

    @pyqtSlot(str)
    def remove_param(self, name):
        self.__exchange.remove_param(name)

    @pyqtSlot(str)
    def set_expression(self, raw):
        self.__exchange.set_expression(raw)

    @pyqtSlot(int, int)
    def set_canvas_size(self, w, h):
        self.__exchange.set_canvas_size(w, h)


class Line:
    __slots__ = ("__points", "__view_area")

    def __init__(self):
        self.__points = []
        self.__view_area = None

    def set_view_area(self, x, y, w, h):
        self.__view_area = (x, y, w, h)

    def add_point(self, x, y):
        self.__points.append(complex(x, y))

    def build(self, cw, ch):
        path = QPainterPath()
        first = True
        for p in self.__points:
            mx = (p.real - self.__view_area[0]) / self.__view_area[2] * cw
            my = (p.imag - self.__view_area[1]) / self.__view_area[3] * ch
            if first:
                first = False
                path.moveTo(mx, ch - my)
            else:
                path.lineTo(mx, ch - my)
        return path


def main(exchange_constructor):
    qapp = QApplication([])
    window = MainWindow(None)
    exchange = ExchangeQtWrapper(exchange_constructor(set_line_signal=window.set_line, q_path_constructor=Line))

    window.scroll_x.connect(exchange.scroll_x)
    window.scroll_y.connect(exchange.scroll_y)
    window.zoom_x.connect(exchange.zoom_x)
    window.zoom_y.connect(exchange.zoom_y)
    window.set_param.connect(exchange.add_param)
    window.remove_param.connect(exchange.remove_param)
    window.set_param.connect(exchange.add_param)
    window.raw_expression_changed.connect(exchange.set_expression)
    window.resized.connect(exchange.set_canvas_size)

    window.show()
    qapp.exec()


if __name__ == "__main__":
    qapp = QApplication([])
    s = MainWindow()
    s.show()
    qapp.exec()
