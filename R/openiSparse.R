
.openiSpase=setClass("openiSpase",
                     slot=c(data="vector",rowInd="vector",colInd="vector",
                            rowNum="numeric",colNum="numeric",
                            offset="numeric",GPUaddress="numeric",status="character"))
openiSpase<-function(data,rowInd,colInd,rowNum,colNum){
  obj=.openiSpase(data=as.double(data),rowInd=as.integer(rowInd),colInd=as.double(colInd),rowNum=as.double(rowNum),colNum=as.double(colNum))
  obj@offset=as.double(0)
  obj@status="CPU memory only"
  obj@GPUaddress=0
  return(obj)
}


setGeneric(name="upload",def=function(obj){standardGeneric("upload")})
setMethod(f="upload",signature = "openiSpase",
          definition=function(obj){
            obj@GPUaddress=as.double(0)
            result=.C("upload",
                      obj@data,obj@rowInd,obj@colInd,
                      as.double(c(length(obj@data),length(obj@rowInd),length(obj@colInd),obj@rowNum,obj@colNum)),
                      obj@offset,obj@GPUaddress)
            obj@GPUaddress=result[[length(result)]]
            obj@status="synchronized"
            return(obj)
          })

setGeneric(name="download",def=function(obj){standardGeneric("download")})
setMethod(f="download",signature = "openiSpase",
          definition=function(obj){
            if(length(obj@GPUaddress)==0){
              stop("The GPU data does not exist")
            }
            result=.C("download",
                      obj@data,obj@rowInd,obj@colInd,obj@GPUaddress)
            obj@data=result[[1]]
            obj@rowInd=result[[2]]
            obj@colInd=result[[3]]
            obj@status="synchronized"
            return(obj)
          })
setGeneric(name="delete",def=function(obj){standardGeneric("delete")})
setMethod(f="delete",signature = "openiSpase",
          definition=function(obj){
            if(length(obj@GPUaddress)==0){
              stop("The GPU data does not exist")
            }
            result=.C("clear",obj@GPUaddress)
            obj@GPUaddress=0
            obj@status="CPU memory only"
            return(obj)
          })
#rowSums=getGeneric(f="rowSums")
setGeneric(name="rowSums")
setMethod(f="rowSums",
          signature = signature(x="openiSpase"),
          definition = function(x,na.rm=FALSE,dims=1){
            if(length(x@GPUaddress)==0){
              stop("The GPU data does not exist")
            }
            sumResult=as.double(rep(0,x@rowNum))
            result=.C("rowSum",
                      sumResult,x@GPUaddress)
            sumResult=result[[1]]
            return(sumResult)
          })
#colSums=getGeneric(f="colSums")
setGeneric(name="colSums")
setMethod(f="colSums",signature = signature(x="openiSpase"),
          definition = function(x,na.rm=FALSE,dims=1){
            if(length(x@GPUaddress)==0){
              stop("The GPU data does not exist")
            }
            sumResult=as.double(rep(0,x@colNum))
            result=.C("colSum",
                      sumResult,x@GPUaddress)
            sumResult=result[[1]]
            return(sumResult)
          })
