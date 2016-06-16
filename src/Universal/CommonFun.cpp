/*
 * CommonFun.cpp
 *
 *  Created on: Oct 15, 2014
 *      Author: qiaohj
 */

#include "CommonFun.h"

void CommonFun::convert2LL(double* x, double* y, const char *fromWkt, const char *toWkt){
	OGRSpatialReference oSourceSRS;
	OGRSpatialReference oTargetSRS;
	char * from_T = const_cast<char *>(fromWkt);
	char * to_T = const_cast<char *>(toWkt);
	oSourceSRS.importFromWkt(&from_T);
	oTargetSRS.importFromWkt(&to_T);
	OGRCoordinateTransformation *poCT;
	poCT = OGRCreateCoordinateTransformation( &oSourceSRS, &oTargetSRS );
	int result = poCT->Transform( 1, x, y );
	if (result==0){
		//LOG(INFO)<<"Failed to convert";
	}
	//LOG(INFO)<<"To LL:"<<result<<" lon:"<<*x<<"; lat:"<<*y;
	delete poCT;
}


double CommonFun::GreatCirleDistanceFast(int x1, int y1, int x2, int y2,
		OGRCoordinateTransformation *poCT, const double* geoTrans, int resolution){
	double x1_km, x2_km, y1_km, y2_km;
	XY2LL(geoTrans, x1, y1, &x1_km, &y1_km);
	XY2LL(geoTrans, x2, y2, &x2_km, &y2_km);

	poCT->Transform( 1, &x1_km, &y1_km);
	poCT->Transform( 1, &x2_km, &y2_km);
	return vincenty_distance(y1_km, x1_km, y2_km, x2_km)/resolution;

}
double CommonFun::GreatCirleDistance(int x1, int y1, int x2, int y2, const char* fromWkt, const char* toWkt, const double* geoTrans, int resolution){
	double x1_km, x2_km, y1_km, y2_km;
	XY2LL(geoTrans, x1, y1, &x1_km, &y1_km);
	XY2LL(geoTrans, x2, y2, &x2_km, &y2_km);
	convert2LL(&x1_km, &y1_km, fromWkt, toWkt);
	convert2LL(&x2_km, &y2_km, fromWkt, toWkt);
	return vincenty_distance(y1_km, x1_km, y2_km, x2_km)/resolution;

}
double CommonFun::deg2rad(double deg)
{
    return (deg * M_PI / 180.0);
}

double CommonFun::haversine_distance(double latitude1, double longitude1, double latitude2,
                          double longitude2)
{
    double lat1 = deg2rad(latitude1);
    double lon1 = deg2rad(longitude1);
    double lat2 = deg2rad(latitude2);
    double lon2 = deg2rad(longitude2);

    double d_lat = abs(lat1 - lat2);
    double d_lon = abs(lon1 - lon2);

    double a = pow(sin(d_lat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(d_lon / 2), 2);

    //double d_sigma = 2 * atan2(sqrt(a), sqrt(1 - a));
    double d_sigma = 2 * asin(sqrt(a));

    return earth_radius_km * d_sigma;
}

double CommonFun::vincenty_distance(double latitude1, double longitude1, double latitude2,
                         double longitude2)
{
	//LOG(INFO)<<"lat1: "<<latitude1<<" lon1: "<<longitude1<<" lat2: "<<latitude2<<" lon2: "<<longitude2;
    double lat1 = deg2rad(latitude1);
    double lon1 = deg2rad(longitude1);
    double lat2 = deg2rad(latitude2);
    double lon2 = deg2rad(longitude2);

    double d_lon = abs(lon1 - lon2);

    // Numerator
    double a = pow(cos(lat2) * sin(d_lon), 2);

    double b = cos(lat1) * sin(lat2);
    double c = sin(lat1) * cos(lat2) * cos(d_lon);
    double d = pow(b - c, 2);

    double e = sqrt(a + d);

    // Denominator
    double f = sin(lat1) * sin(lat2);
    double g = cos(lat1) * cos(lat2) * cos(d_lon);

    double h = f + g;

    double d_sigma = atan2(e, h);

    return earth_radius_km * d_sigma;
}

std::string CommonFun::readFile(const char* path) {
    std::FILE* file = fopen(path, "rb");
    if (!file)
        return std::string("");
    std::fseek(file, 0, SEEK_END);
    long size = ftell(file);
    std::fseek(file, 0, SEEK_SET);
    std::string text;
    char* buffer = new char[size + 1];
    buffer[size] = 0;
    if (fread(buffer, 1, size, file) == (unsigned long) size)
        text = buffer;
    fclose(file);
    delete[] buffer;
    return text;
}

Json::Value CommonFun::readJson(const char* path) {
    Json::Features features;
    Json::Reader reader(features);
    Json::Value root;
    std::string input = readFile(path);
    bool parsingSuccessful = reader.parse(input, root);
    if (parsingSuccessful) {
        return root;
    } else {
        return root;
    }
}
std::string CommonFun::removeSuffix(const std::string& path, const std::string& extension) {
    if (extension.length() >= path.length())
        return std::string("");
    std::string suffix = path.substr(path.length() - extension.length());
    if (suffix != extension)
        return std::string("");
    return path.substr(0, path.length() - extension.length());
}
void CommonFun::writeFile(const std::string s, const char* path) {
    std::ofstream outfile(path);
    if (!outfile.is_open()) {
        std::cerr << "Couldn't open " << path << std::endl;
    }
    outfile << s << std::endl;
    outfile.close();
}
void CommonFun::writeFile(const std::vector<std::string> s, const char* path) {
    std::string joined = boost::algorithm::join(s, "\n");
    writeFile(joined, path);
}
void CommonFun::XY2LL(const double* adfGeoTransform, const unsigned x,
        const unsigned y, double* longitude, double* latitude) {
	//LOG(INFO)<<"x:"<<x<<" y:"<<y;

    *longitude = adfGeoTransform[0] + adfGeoTransform[1] * (x + .5f)
            + adfGeoTransform[2] * (y + .5f);
    *latitude = adfGeoTransform[3] + adfGeoTransform[4] * (x + .5f)
            + adfGeoTransform[5] * (y + .5f);

    //LOG(INFO)<<"x:"<<x<<" y:"<<y<<" converted x:"<<*longitude<<" converted y:"<<*latitude;
}

void CommonFun::LL2XY(const double* adfGeoTransform, const double longitude,
        const double latitude, unsigned* x, unsigned* y) {
    *x = (unsigned) ((longitude - adfGeoTransform[0]) / adfGeoTransform[1]);
    *y = (unsigned) ((latitude - adfGeoTransform[3]) / adfGeoTransform[5]);
}
void CommonFun::createFolder(const char* path) {
    if (!(boost::filesystem::exists(path))) {
        boost::filesystem::create_directory(path);
    }
}

std::string CommonFun::fixedLength(int value, int digits = 3) {
    unsigned int uvalue = value;
    if (value < 0) {
        uvalue = -uvalue;
    }
    std::string result;
    while (digits-- > 0) {
        result += ('0' + uvalue % 10);
        uvalue /= 10;
    }
    if (value < 0) {
        result += '-';
    }
    std::reverse(result.begin(), result.end());
    return result;
}


/**
 * Returns the peak (maximum so far) resident set size (physical
 * memory use) measured in bytes, or zero if the value cannot be
 * determined on this OS.
 */
size_t CommonFun::getPeakRSS() {
    struct rusage rusage;
    getrusage( RUSAGE_SELF, &rusage);
    return (size_t) (rusage.ru_maxrss * 1024L);
}

/**
 * Returns the current resident set size (physical memory use) measured
 * in bytes, or zero if the value cannot be determined on this OS.
 */
size_t CommonFun::getCurrentRSS() {
    long rss = 0L;
    FILE* fp = NULL;
    if ((fp = fopen("/proc/self/statm", "r")) == NULL)
        return (size_t) 0L; /* Can't open? */
    if (fscanf(fp, "%*s%ld", &rss) != 1) {
        fclose(fp);
        return (size_t) 0L; /* Can't read? */
    }
    fclose(fp);
    return (size_t) rss * (size_t) sysconf( _SC_PAGESIZE) / (1024 * 1024);

}
size_t CommonFun::writeMemoryUsage(unsigned line, bool is, size_t last){
    if (is){
        boost::this_thread::sleep( boost::posix_time::milliseconds(1) );
        size_t current = getCurrentRSS();
        if ((current - last)!=0){
            LOG(INFO)<<"Memory usage " << fixedLength(line, 3)<< ": " << (current - last);
        }
        return current;
    }
    return 0;
}
