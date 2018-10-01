library("Matrix")
lib_file="src/test_openCL.dll"
dyn.load(lib_file)
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
