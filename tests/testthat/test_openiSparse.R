
test_that("Matrix upload and download",{
  lib_file="src/openTrick.dll"
  dyn.load(lib_file)
  k=100
  test.data=sparseData(row=k,col=k,nonzero=k*k/2)
  myopencl=openiSpase(test.data$dataframe,test.data$rowind,test.data$colind,test.data$rowNum,test.data$colNum)
  myopencl=upload(myopencl)
  myopencl@data=as.double(rep(0,length(test.data$dataframe)))
  myopencl@rowInd=as.integer(rep(0,length(test.data$dataframe)))
  myopencl@colInd=as.double(rep(0,length(test.data$colind)))
  myopencl=download(myopencl)
  expect_equal(myopencl@data,test.data$dataframe)
  expect_equal(myopencl@rowInd,test.data$rowind)
  expect_equal(myopencl@colInd,test.data$colind)
  delete(myopencl)
  dyn.unload(lib_file)
})


test_that("Matrix sum",{
  lib_file="src/openTrick.dll"
  dyn.load(lib_file)
  k=100
  test.data=sparseData(row=k,col=k,nonzero=k*k/2)
  myopencl=openiSpase(test.data$dataframe,test.data$rowind,test.data$colind,test.data$rowNum,test.data$colNum)
  myopencl=upload(myopencl)
  col_result=colSums(myopencl)
  row_result=rowSums(myopencl)
  expect_equal(col_result,base::colSums(test.data$dataMatrix))
  expect_equal(row_result,base::rowSums(test.data$dataMatrix))
  dyn.unload(lib_file)
})


