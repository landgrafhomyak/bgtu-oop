plugins {
    kotlin("multiplatform") version "2.0.0"
}

group = "ru.landgrafhomyak.studying.bgtu"
version = "1.0-SNAPSHOT"

repositories {
    mavenCentral()
}

kotlin {
    jvm()
    js(IR) {
        browser{
            webpackTask {
                outputs.upToDateWhen { false }
            }
        }
        binaries.executable()
    }
}