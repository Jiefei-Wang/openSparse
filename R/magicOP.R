.magicOP=setClass("magicOP",
                  slot=c(operation="vector"))

magicOP<-function(num=NULL){
  if(is.null(num))
    obj=.magicOP(operation=c(""))
  else
    obj=.magicOP(operation=c(as.character(num)))
}
