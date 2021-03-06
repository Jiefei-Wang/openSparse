% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/GPUresourcesManager.R
\docType{data}
\name{.resourceManager}
\alias{.resourceManager}
\title{A list of functions that can be used to manage the resources in GPU}
\format{An object of class \code{list} of length 6.}
\usage{
.resourceManager
}
\description{
'addAddress', 'getAddress' and 'releaseAddress' are designed to be used inside the function call, user should not call them
The function 'releaseAll' will release all the resources in GPU.
'getGPUusage' can be used to check the number of GPU data
}
\keyword{datasets}
