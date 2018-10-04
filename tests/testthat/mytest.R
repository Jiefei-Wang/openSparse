



dyn.load(.parms$getLib())
mydata=sparseData()

myclass=openiSpase(mydata$dataframe,mydata$rowind,mydata$colind,mydata$size[4],mydata$size[5])
myclass=upload(myclass)
sum((colSums(myclass)-colSums(mydata$dataMatrix))^2)
sum((rowSums(myclass)-rowSums(mydata$dataMatrix))^2)
myclass=download(myclass)
dyn.unload(.parms$getLib())

dyn.load(.parms$getLib())
library(rhdf5)
library(ExperimentHub)
library(HDF5Array)
library("tictoc")
hub=ExperimentHub::ExperimentHub()
fl=hub[["EH1039"]]
mydata=openSparse(fl)
tic()
res=rowSums(mydata)
toc()
mydata1=TENxMatrix(fl, group="mm10")
tic()
res1=colSums(mydata1)
toc()

dyn.unload(.parms$getLib())

