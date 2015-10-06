library(raster)
library(stringr)

for (i in seq(100, 120000, by=100)){
  print(i)
  r<-raster(paste("/home/huijieqiao/temp/scenario/01/groupsmap/", str_pad(i, 7, pad = "0"), ".tif", sep=""))
  plot(r)
}



r<-raster("/home/huijieqiao/temp/scenario/01/groupsmap/0048100.tif")
plot(r)

r<-raster("/home/huijieqiao/temp/scenario/01/01/groupsmap/0048200.tif")
plot(r)

r<-raster("/home/huijieqiao/temp/scenario/01/02/groupsmap/0048200.tif")
plot(r)

r<-raster("/home/huijieqiao/temp/scenario/01/03/groupsmap/0048200.tif")
plot(r)

r<-raster("/home/huijieqiao/temp/scenario/01/04/groupsmap/0048200.tif")
plot(r)

r<-raster("/home/huijieqiao/temp/scenario/01/05/groupsmap/0048200.tif")
plot(r)

r<-raster("/home/huijieqiao/temp/scenario/01/06/groupsmap/0048200.tif")
plot(r)

r<-raster("/home/huijieqiao/temp/scenario/01/07/groupsmap/0048200.tif")
plot(r)

r<-raster("/home/huijieqiao/temp/scenario/01/08/groupsmap/0048200.tif")
plot(r)

r<-raster("/home/huijieqiao/temp/scenario/01/09/groupsmap/0048200.tif")
plot(r)

r<-raster("/home/huijieqiao/temp/scenario/01/01/groupsmap/0024900.tif")
plot(r)
r<-raster("/home/huijieqiao/temp/scenario/01/groupsmap/0024800.tif")
plot(r)


r<-raster("/home/huijieqiao/temp/scenario/01/suitable.tif")
plot(r)

r<-raster("/home/huijieqiao/temp/scenario/01/02/groupsmap/0024900.tif")
plot(r)


r<-raster("/home/huijieqiao/temp/scenario/01/groupsmap/0044900.tif")
plot(r)

r<-raster("/home/huijieqiao/temp/scenario/01/01/groupsmap/0045400.tif")
plot(r)

r<-raster("/home/huijieqiao/temp/scenario/01/02/groupsmap/0045000.tif")
plot(r)

r<-raster("/home/huijieqiao/temp/scenario/01/03/groupsmap/0045000.tif")
plot(r)

r<-raster("/home/huijieqiao/NB_NEW_Result/scenario.seed_587.dispersal_good.nb_large/87/suitable.tif")
plot(r)

r<-raster("/home/huijieqiao/NB_NEW_Result/scenario.seed_877.dispersal_poor.nb_large/77/suitable.tif")
plot(r)

r<-raster("/home/huijieqiao/NB_NEW_Result/scenario.seed_877.dispersal_poor.nb_large/77/groupsmap/0012000.tif")
plot(r)
