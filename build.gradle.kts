plugins {
    kotlin("multiplatform") version "2.0.0"
}

group = "ru.landgrafhomyak.studying.bgtu"
version = "1.0-SNAPSHOT"

repositories {
    mavenCentral()
}


val browserDistributionDir = projectDir.resolve("out/browser")
val assembleBrowserTask = tasks.create<Copy>("assembleBrowserDistribution") {
    group = "application"
    dependsOn(tasks.named("browserBrowserWebpack"::equals))
    from(projectDir.resolve("src/browserMain/static"))
    into(browserDistributionDir)
}

kotlin {
    explicitApi()
    jvm()
    js("browser", IR) {
        browser {
            commonWebpackConfig {
                outputFileName = "app.js"
                outputPath = browserDistributionDir
            }
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