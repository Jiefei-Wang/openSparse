library(BiocGenerics)



.openSparse=setClass("openSparse",
                     slot=c(file="character",fileDescription="list",data="vector",rowInd="vector",colInd="vector",
                            colDiff="vector",rowNum="numeric",colNum="numeric",size="vector"))


openSparse<-function(file){
  fileData=h5ls(file)
  rowNum=as.integer(fileData$dim[which(fileData$name=="genes")])
  colNum=as.integer(fileData$dim[which(fileData$name=="barcodes")])
  colInd=h5read(fl, "mm10/indptr", bit64conversion="double")
  rowInd.len=as.double(fileData$dim[which(fileData$name=="indices")])
  colInd.len=length(colInd)
  colDiff=diff(colInd)
  obj=.openSparse(file=file,fileDescription=fileData,colInd=colInd,
                  colDiff=colDiff,rowNum=rowNum,colNum=colNum,size=c(rowInd.len,colInd.len))
  return(obj)
}


setGeneric(name="rowSums")
setMethod(f="rowSums",signature=signature(x="openSparse"),
          definition = function(x,na.rm=FALSE,dims=1){
            i1=1
            i2=1
            rowRes=rep(0,x@rowNum)
            pb=txtProgressBar(min = 0, max = length(x@colInd))
            repeat{
              gc()
              repeat{
                if(x@colInd[i2]-x@colInd[i1]>chunkSize||i2==length(x@colInd))
                  break
                i2=i2+1
              }
              colInd_block=x@colInd[i1:i2]-x@colInd[i1]
              rowInd_block=h5read(x@file, "mm10/indices",start=x@colInd[i1]+1,count=x@colInd[i2]-x@colInd[i1])
              data_block=h5read(x@file, "mm10/data",start=x@colInd[i1]+1,count=x@colInd[i2]-x@colInd[i1])
              block_sparse=openiSpase(data_block,rowInd_block,colInd_block,x@rowNum,x@colNum)
              block_sparse=upload(block_sparse)
              rowRes=rowRes+rowSums(block_sparse)
              block_sparse=delete(block_sparse)
              i1=i2
              setTxtProgressBar(pb, i1)
              if(i1==length(x@colInd))
                break;
            }
            close(pb)
            return(rowRes)
          })

setGeneric(name="colSums")
setMethod(f="colSums",signature=signature(x="openSparse"),
          definition = function(x,na.rm=FALSE,dims=1){
            i1=1
            i2=1
            colRes=rep(0,x@colNum)
            pb=txtProgressBar(min = 0, max = length(x@colInd))
            repeat{
              gc()
              repeat{
                i2=i2+1
                if(x@colInd[i2]-x@colInd[i1]>chunkSize||i2==length(x@colInd))
                  break
              }
              colInd_block=x@colInd[i1:i2]-x@colInd[i1]
              rowInd_block=h5read(x@file, "mm10/indices",start=x@colInd[i1]+1,count=x@colInd[i2]-x@colInd[i1])
              data_block=h5read(x@file, "mm10/data",start=x@colInd[i1]+1,count=x@colInd[i2]-x@colInd[i1])
              block_sparse=openiSpase(data_block,rowInd_block,colInd_block,x@rowNum,i2-i1)
              block_sparse=upload(block_sparse)
              colRes[i1:(i2-1)]=colSums(block_sparse)
              block_sparse=delete(block_sparse)
              i1=i2
              setTxtProgressBar(pb, i1)
              if(i1==length(x@colInd))
                break;
            }
            close(pb)
            return(colRes)
          })



#save(rowRes,file="temp")

