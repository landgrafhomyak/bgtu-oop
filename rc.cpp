#include <cstdio>

int main(int argc, char const *const *const argv) {
    if (argc != 5) {
        std::printf("Usage: rc {include} {name} {src_file} {dst_file}\n");
        return 1;
    }

    char const *const include = argv[1];
    char const *const var_name = argv[2];

    std::FILE *fin = std::fopen(argv[3], "rt");
    if (fin == nullptr) {
        std::printf("Can't open input file\n");
        return 1;
    }

    std::FILE *fout = std::fopen(argv[4], "wt");
    if (fout == nullptr) {
        std::fclose(fin);
        std::printf("Can't open output file\n");
        return 1;
    }

    size_t read_cnt;
    char chunk[1024];

    if (std::fprintf(fout, "#include %s\n\nconst char %s[] = {", include, var_name) < 0) {
        std::printf("Error while writing\n");
        goto ERR;
    }
    while (!std::feof(fin)) {
        read_cnt = std::fread(chunk, sizeof(char), 1024, fin);
        if (std::ferror(fin)) {
            std::printf("Error while reading\n");
            goto ERR;
        }

        for (size_t i = 0; i < read_cnt; i++) {
            if (std::fprintf(fout, "%d,", chunk[i]) < 0) {
                std::printf("Error while writing\n");
                goto ERR;
            }
        }
    }
    std::fclose(fin);
    if (std::fprintf(fout, "0};\n") < 0) {
        std::printf("Error while writing\n");
        goto ERR;
    }
    std::fclose(fout);
    return 0;


    ERR:
    std::fprintf(fout, "\n#error \"error while generating file\"");
    std::fclose(fout);
    std::fclose(fin);
    return 1;
}