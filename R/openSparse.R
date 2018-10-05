#' @useDynLib openSparse
#' @importFrom BiocGenerics colSums rowSums
#' @importFrom methods new
#' @importFrom rhdf5 h5ls h5read

.openSparse=setClass(
  "openSparse",
  slot=c(
    file="character",fileDescription="list",
    data="vector",rowInd="vector",colInd="vector",
    colDiff="vector",rowNum="numeric",colNum="numeric",size="vector"
  )
)

#' Create an 'openSparse' object
#'
#' The 'openSparse' object provide a variaty of matrix operation on the sparse data.
#'
#' @param  file character(1) path to hdf5 file containing a CSC formated sparse matrix.
#'
#' @return An object of class 'openSparse'
#'
#' @examples
#' library(rhdf5)
#' library(ExperimentHub)
#' library(HDF5Array)
#' library("tictoc")
#' hub=ExperimentHub::ExperimentHub()
#' fl=hub[["EH1039"]]
#' mydata=openSparse(fl)
#'
#' @export
openSparse<-function(file){
  fileData=h5ls(file)
  rowNum=as.integer(fileData$dim[which(fileData$name=="genes")])
  colNum=as.integer(fileData$dim[which(fileData$name=="barcodes")])
  colInd=h5read(file, "mm10/indptr", bit64conversion="double")
  rowInd.len=as.double(fileData$dim[which(fileData$name=="indices")])
  colInd.len=length(colInd)
  colDiff=diff(colInd)

  .openSparse(file=file,fileDescription=fileData,colInd=colInd,
                  colDiff=colDiff,rowNum=rowNum,colNum=colNum,size=c(rowInd.len,colInd.len))
}


#' @export
setMethod(
  f="rowSums",signature=signature(x="openSparse"),
  definition = function(x,na.rm=FALSE,dims=1)
  {
    i1=1
    i2=1
    rowRes=rep(0,x@rowNum)
    pb=txtProgressBar(min = 0, max = length(x@colInd))
    repeat{
      repeat{
        if(x@colInd[i2]-x@colInd[i1]>.parms$getChunkSize()||i2==length(x@colInd))
          break
        i2=i2+1
      }
      colInd_block=x@colInd[i1:i2]
      rowInd_block=h5read(x@file, "mm10/indices",start=x@colInd[i1]+1,count=x@colInd[i2]-x@colInd[i1])
      data_block=h5read(x@file, "mm10/data",start=x@colInd[i1]+1,count=x@colInd[i2]-x@colInd[i1])
      block_sparse=openiSparse(data_block,rowInd_block,colInd_block,x@rowNum,x@colNum)
      block_sparse=upload(block_sparse)
      rowRes=rowRes+rowSums(block_sparse)
      block_sparse=delete(block_sparse)
      i1=i2
      setTxtProgressBar(pb, i1)
      if(i1==length(x@colInd))
        break;
    }
    close(pb)

    rowRes
  })

#' @importFrom utils setTxtProgressBar txtProgressBar
#' @export
setMethod(
  f="colSums",signature=signature(x="openSparse"),
  definition = function(x,na.rm=FALSE,dims=1)
  {
    i1=1
    i2=1
    colRes=rep(0,x@colNum)
    pb=txtProgressBar(min = 0, max = length(x@colInd))
    repeat{
      repeat{
        i2=i2+1
        if(x@colInd[i2]-x@colInd[i1]>.parms$getChunkSize()||i2==length(x@colInd))
          break
      }
      colInd_block=x@colInd[i1:i2]
      rowInd_block=h5read(x@file, "mm10/indices",start=x@colInd[i1]+1,count=x@colInd[i2]-x@colInd[i1])
      data_block=h5read(x@file, "mm10/data",start=x@colInd[i1]+1,count=x@colInd[i2]-x@colInd[i1])
      block_sparse=openiSparse(data_block,rowInd_block,colInd_block,x@rowNum,i2-i1)
      block_sparse=upload(block_sparse)
      colRes[i1:(i2-1)]=colSums(block_sparse)
      block_sparse=delete(block_sparse)
      i1=i2
      setTxtProgressBar(pb, i1)
      if(i1==length(x@colInd))
        break;
    }
    close(pb)

    colRes
  })



#save(rowRes,file="temp")

