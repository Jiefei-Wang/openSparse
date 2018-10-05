#' A list of functions that can be used to manage the resources in GPU
#'
#' @description
#' 'addAddress', 'getAddress' and 'releaseAddress' are designed to be used inside the function call, user should not call them
#' The function 'releaseAll' will release all the resources in GPU.
#' 'getGPUusage' can be used to check the number of GPU data
#'
#' @export
.resourceManager<-local({
  e=new.env()
  e$N=100
  e$incremental=100
  e$address=as.double(rep(0,e$N))
  e$empInd=1:e$N
  list(
    addAddress=function(ad){
      if(ad==0) stop("GPU memory does not exist")
      if(length(e$empInd)==0){
        e$N=e$N+e$incremental
        e$address=as.double(c(e$address,rep(0,e$incremental)))
        e$empInd=(length(e$address)-e$incremental+1):length(e$address)
      }
      ind=e$empInd[1]
      e$address[ind]=ad
      e$empInd=e$empInd[-1]
      return(ind)
    },
    getAddress=function(ind){
      if(ind%in%e$empInd||ind>length(e$address)) stop("GPU memory does not exist")
      return(e$address[ind])
    },
    releaseAddress=function(ind){
      if(ind%in%e$empInd||ind>length(e$address)){
        warning("GPU memory has already been free")
        return()
      }
      .C("clear",e$address[ind])
      e$empInd=c(e$empInd,ind)
    },
    releaseAll=function(){
      inUsedInd=1:length(e$address)
      inUsedInd=as.double(subset(inUsedInd,!(inUsedInd%in%e$empInd)))
      for(i in inUsedInd){
        .C("clear",i)
      }
      e$empInd=1:length(e$address)
    },
    getGPUusage=function(){
      message(paste0("Memory container length: ",e$N))
      message(paste0("Currect registered GPU memory: ",e$N-length(e$empInd)))
    },
    deleteEnv=function(){
      rm(list =ls(envir = e),envir=e)
    }
  )
})

#' The function is used to obtain all the opencl-enable devices
#' @export
getDeviceList=function(){
  .C("getDeviceList")
  invisible()
}
#' Get the ith device information, call 'getDeviceList()' first to figure out the index before use this function
#' @param i numeric The device index
#' @export
getDeviceInfo=function(i){
  .C("getDeviceInfo",as.integer(i))
  invisible()
}
#' Get the device detailed information, call 'getDeviceList()' first to figure out the index before use this function
#' @param i The device index
#' @export
getDeviceDetail=function(i){
  .C("getDeviceDetail",as.integer(i))
  invisible()
}
#' Get the current used device
#' @export
getCurDevice=function(){
  .C("getCurDevice")
  invisible()
}
#' Set which device will be used in the opencl, call 'getDeviceList()' first to figure out the index before use this function
#' @param i numeric The device index
#' @export
setDevice=function(i){
  .resourceManager$releaseAll()
  .C("setDevice",as.integer(i))
  invisible()
}
