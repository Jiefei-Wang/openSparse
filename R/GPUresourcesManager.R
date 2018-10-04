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


getDeviceList=function(){
  .C("getDeviceList")
  a=1
}
getDeviceInfo=function(i){
  .C("getDeviceInfo",as.integer(i))
  a=1
}
getDeviceDetail=function(i){
  .C("getDeviceDetail",as.integer(i))
  a=1
}
getCurDevice=function(){
  .C("getCurDevice")
  a=1
}
setDevice=function(i){
  .C("setDevice",as.integer(i))
  a=1
}
