



lib="src/openTrick.dll"
dyn.load(lib)
mydata=sparseData()

myclass=openiSpase(mydata$dataframe,mydata$rowind,mydata$colind,mydata$size[4],mydata$size[5])
myclass=upload(myclass)
sum((colSums(myclass)-colSums(mydata$dataMatrix))^2)
sum((rowSums(myclass)-rowSums(mydata$dataMatrix))^2)
myclass=download(myclass)

dyn.unload(lib)

sumResult=as.double(rep(0,100))
.C("rowSum",
   sumResult,myclass@GPUaddress)


lib_file="src/test_openCL.dll"
dyn.load(lib_file)
dyn.unload(lib_file)
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


