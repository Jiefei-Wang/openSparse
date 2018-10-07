.openiSparse=setClass("openiSparse",
                     slot=c(data="vector",rowInd="vector",colInd="vector",
                            rowNum="numeric",colNum="numeric",
                            offset="numeric",GPUaddress="numeric",status="character"))
.GPUaddress<-function(x){
  if(x@GPUaddress==0){
    stop("The GPU data does not exist")
  }
  .resourceManager$getAddress(x@GPUaddress)
}
.data<-function(x)x@data
'.data<-'<-function(x,value){
  x@data<-value
  x
}

#' @export
openiSparse<-function(data,rowInd,colInd,rowNum,colNum){
  obj=.openiSparse(
    data=as.double(data),rowInd=as.integer(rowInd),
    colInd=as.double(colInd-colInd[1]),rowNum=as.double(rowNum),
    colNum=as.double(colNum)
    )
  obj@offset=as.double(colInd[1])
  obj@status="CPU memory only"
  obj@GPUaddress=0L

  obj
}



setGeneric(name="upload",def=function(obj){standardGeneric("upload")})

#' @export
setMethod(
  f="upload",signature = "openiSparse",
  definition=function(obj)
  {
    obj@GPUaddress=0L
    result=.C("upload",
              obj@data,obj@rowInd,obj@colInd,
              as.double(
                c(length(.data(obj)),length(obj@rowInd),length(obj@colInd),
                          obj@rowNum,obj@colNum)
                ),obj@offset,as.double(0))
    obj@GPUaddress=.resourceManager$addAddress(result[[length(result)]])
    obj@status="synchronized"

    obj
  })

setGeneric(name="download",def=function(obj){standardGeneric("download")})

#' @export
setMethod(
  f="download",signature = "openiSparse",
  definition=function(obj){
    result=.C("download",
              obj@data,obj@rowInd,obj@colInd,.GPUaddress(obj))
    .data(obj)=result[[1]]
    obj@rowInd=result[[2]]
    obj@colInd=result[[3]]
    obj@status="synchronized"
    return(obj)
  })


setGeneric(name="delete",def=function(obj){standardGeneric("delete")})

#' @export
setMethod(
  f="delete",signature = "openiSparse",
  definition=function(obj){
    if(obj@GPUaddress==0){
      stop("The GPU data does not exist")
    }
    .resourceManager$releaseAddress(obj@GPUaddress)
    obj@GPUaddress=0L
    obj@status="CPU memory only"
    return(obj)
  })


#setGeneric(name="rowSums")

#' @export
setMethod(
  f="rowSums",
  signature = signature(x="openiSparse"),
  definition = function(x,na.rm=FALSE,dims=1){
    rowSumsOP(x,"")
  })

#setGeneric(name="colSums")

#' @export
setMethod(
  f="colSums",signature = signature(x="openiSparse"),
  definition = function(x,na.rm=FALSE,dims=1){
    colSumsOP(x,"")
  })

setGeneric(name="rowSumsOP",def=function(x,op){standardGeneric("rowSumsOP")})

#' @export
setMethod(
  f="rowSumsOP",
  signature = signature(x="openiSparse",op="character"),
  definition = function(x,op){
    sumResult=as.double(rep(0,x@rowNum))
    result=.C("rowSum",sumResult,.GPUaddress(x),op)
    sumResult=result[[1]]

    sumResult
  })

setGeneric(name="colSumsOP",def=function(x,op){standardGeneric("colSumsOP")})

#' @export
setMethod(
  f="colSumsOP", signature = signature(x="openiSparse",op="character"),
  definition = function(x,op){
    sumResult=as.double(rep(0,x@colNum))
    result=.C("colSum",sumResult,.GPUaddress(x),op)
    sumResult=result[[1]]

    sumResult
  }
)


