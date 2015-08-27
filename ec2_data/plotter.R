#library(lattice)

plotchunks <- function(file, title, maxi) {
    names = c("P = 1", "P = 2")
    colors = c("red", "green", "blue", "yellow", "pink", "violet", "cyan", "orange")
    data <- read.table(file, sep=" ", header=TRUE)
    print(data)
    avg <- colMeans(data)
    print(avg)


    barplot(avg,
        main = title,
        ylab="Time (seconds)",
        las = 2,
        col=colors,
        beside=TRUE,
        ylim = c(0, maxi))
}

plotfull <- function(file, title, maxi) {
    names = c("P = 1", "P = 2")
    colors = c("red", "green", "blue", "pink", "violet", "cyan")
    data <- read.table(file, sep=" ", header=TRUE)
    print(data)
    avg <- colMeans(data)
    print(avg)


    barplot(avg,
        main = title,
        ylab="Time (seconds)",
        las = 2,
        col=colors,
        beside=TRUE,
        ylim = c(0, maxi))
}

old.par <- par(mfrow=c(1, 2))
plotchunks("8mb.txt", "8MB chunks", 30)
plotfull("48mb.txt", "48MB full data", 30)
par(old.par)

old.par <- par(mfrow=c(1, 2))
plotchunks("16mb.txt", "16MB chunks", 50)
plotfull("96mb.txt", "96MB full data", 50)
par(old.par)

old.par <- par(mfrow=c(1, 2))
plotchunks("32mb.txt", "32MB chunks", 90)
plotfull("192mb.txt", "192MB full data", 90)
par(old.par)
