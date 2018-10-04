#library.dynam(.parms$getLibName(),pkgname,.parms$getLibPath())
#dyn.load(.parms$getLibPath())

.parms<-local({
  e=new.env()
  e$chunkSize=50000000
  e$dll="src/openSparse.dll"
  e$so="src/openSparse.so"
  list(
    getChunkSize=function(){e$chunkSize},
    setChunkSize=function(size){e$chunkSize=size},
    getLibPath=function(){
      switch(Sys.info()[['sysname']],
             Windows= {e$dll},
             Linux  = {e$so},
             Darwin = {print("Mac is not supported")})},
    getLibName=function(){"openSparse"},
    deleteEnv=function(){rm(list =ls(envir = e),envir=e)}
  )
})
.onLoad<-function(libname, pkgname){

}

.onUnload<-function(libpath){
  library.dynam.unload(.parms$getLibName(),libpath)
  .parms$deleteEnv()
  .resourceManager$deleteEnv()
}

sparseData<-function(row=10,col=10,nonzero=5){
  library("Matrix")
  mydata=matrix(0,row,col)
  mydata[sample(1:length(mydata),nonzero)]=rbinom(nonzero,10,0.5)+1
  m <- Matrix(mydata,sparse=T)

  dataframe=m@x
  rowind=m@i
  colind=m@p
  list(dataframe=dataframe,rowind=rowind,colind=colind,
       rowNum=row,colNum=col,dataMatrix=mydata,
       sparseMatrix=m
  )
}
