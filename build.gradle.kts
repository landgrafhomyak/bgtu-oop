plugins {
    kotlin("multiplatform") version "2.0.0"
}

group = "ru.landgrafhomyak.studying.bgtu"
version = "1.0-SNAPSHOT"

repositories {
    mavenCentral()
}

kotlin {
    explicitApi()
    jvm()
    js("browser", IR) {
        browser {
            webpackTask {
                outputs.upToDateWhen { false }
            }
        }
        binaries.executable()
    }

    sourceSets {
        val browserMain by getting {
            resources.srcDir(projectDir.resolve("./src/browserMain/static"))
        }
    }
}