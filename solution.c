#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

int32_t str2Int32_t(unsigned char *str);
uint64_t str2Uint64_t(uint8_t *str);
void ASCII_Read(uint8_t *str, FILE *VehiclePositions);
void byteSwap(uint8_t *str);
float str2Float(unsigned char *str) ;

//Vehicle information struct
//Registration string is sized according to conservative maximum
 typedef struct vehicle{

        //Position ID
        uint8_t Position_ID_buff[4];
        int32_t Position_ID;

        //Vehicle Registration
        uint8_t Vehicle_Registration[20];

        //Latitude
        float Latitude;
        uint8_t Latitude_buff[4];

        //Longitude
        float Longitude;
        uint8_t Longitude_buff[4];

        //Timestamp
        uint64_t Timestamp;
        uint8_t Timestamp_buff[8];
    } VEHICLE;

//Output and comparison struct
typedef struct coords{
        //Target latitude
        float Lat;

        //Target longitude
        float Long;

        //Current closest position
        float closest_Lat;
        float closest_Long;
        float closest_Distance_sq;

        //Distance for current sample
        float current_Distance_sq;

        //Position ID of the closest position
        int32_t closest_Position_ID;
    } COORDS;


int main()
{
    //Start measuring execution time 
    clock_t start, end;
    double duration = 0;
    start = clock();

    //Declare array of COORD structs to save targets
    //Initialise closest_Distance_sq as low as possible, if an answer is = to this value then it should be increased
    COORDS co_ordinates[10];

    co_ordinates[0].Lat = 34.544909;
    co_ordinates[0].Long = -102.100843;
    co_ordinates[0].closest_Distance_sq = 0.001;

    co_ordinates[1].Lat = 32.345544;
    co_ordinates[1].Long = -99.123124;
    co_ordinates[1].closest_Distance_sq = 0.001;

    co_ordinates[2].Lat = 33.234235;
    co_ordinates[2].Long = -100.214124;
    co_ordinates[2].closest_Distance_sq = 0.001;

    co_ordinates[3].Lat = 35.195739;
    co_ordinates[3].Long = -95.348899;
    co_ordinates[3].closest_Distance_sq = 0.001;

    co_ordinates[4].Lat = 31.895839;
    co_ordinates[4].Long = -97.789573;
    co_ordinates[4].closest_Distance_sq = 0.001;

    co_ordinates[5].Lat = 32.895839;
    co_ordinates[5].Long = -101.789573;
    co_ordinates[5].closest_Distance_sq = 0.001;

    co_ordinates[6].Lat = 34.115839;
    co_ordinates[6].Long = -100.225732;
    co_ordinates[6].closest_Distance_sq = 0.001;

    co_ordinates[7].Lat = 32.335839;
    co_ordinates[7].Long = -99.992232;
    co_ordinates[7].closest_Distance_sq = 0.001;

    co_ordinates[8].Lat = 33.535339;
    co_ordinates[8].Long = -94.792232;
    co_ordinates[8].closest_Distance_sq = 0.001;

    co_ordinates[9].Lat = 32.234235;
    co_ordinates[9].Long = -100.222222;
    co_ordinates[9].closest_Distance_sq = 0.001;



    //Open "VehiclePostions.dat" data file
    FILE *VehiclePositions;
    VehiclePositions = fopen("VehiclePositions.dat", "rb");

    //Check for successful open
    if(VehiclePositions == NULL){
        printf("Error: %d (%s)\n", errno, strerror(errno));
    }

    //Declare a test instance of the Vehicle structure: car_1
    VEHICLE car_1;
    car_1.Position_ID = 0;

    //Loop over samples (consider while loop without magic number for future)
    for(int i = 0; i<2000000; i++){


    //Read Position ID into buffer and convert to Int32_t
    fread(car_1.Position_ID_buff, sizeof(car_1.Position_ID_buff), 1, VehiclePositions);
    car_1.Position_ID = str2Int32_t(car_1.Position_ID_buff);


    //Read vehicle registration number
    ASCII_Read(car_1.Vehicle_Registration, VehiclePositions);



    //Read latitude
    fread(car_1.Latitude_buff, sizeof(car_1.Latitude), 1, VehiclePositions);
    car_1.Latitude = str2Float(car_1.Latitude_buff);



    //Read longitude
    fread(car_1.Longitude_buff, sizeof(car_1.Longitude), 1, VehiclePositions);
    car_1.Longitude = str2Float(car_1.Longitude_buff);


    //Read time
    fread(car_1.Timestamp_buff, sizeof(car_1.Timestamp), 1, VehiclePositions);
    car_1.Timestamp = str2Uint64_t(car_1.Timestamp_buff);


    for(int j = 0; j < 10; j++){

        //No need to take root for distance comparisons
        co_ordinates[j].current_Distance_sq = (car_1.Latitude - co_ordinates[j].Lat)*(car_1.Latitude - co_ordinates[j].Lat) + (car_1.Longitude - co_ordinates[j].Long)*(car_1.Longitude - co_ordinates[j].Long);

        //If a distance closer to target is found, replace current "closest" values
        if(co_ordinates[j].current_Distance_sq < co_ordinates[j].closest_Distance_sq){
            co_ordinates[j].closest_Distance_sq = co_ordinates[j].current_Distance_sq;
            co_ordinates[j].closest_Lat = car_1.Latitude;
            co_ordinates[j].closest_Long = car_1.Longitude;
            co_ordinates[j].closest_Position_ID = car_1.Position_ID;

            /*//Tracking function for debugging
            if(j == 0){
                //printf("%f\n", co_ordinates[j].closest_Distance_sq);
            }
            */
        }
    }
    }

    //Output final values
    for(int i = 0; i < 10; i++){

            printf("Closest Distance (squared) for Coord %d was: %f\n", i,co_ordinates[i].closest_Distance_sq);
            printf("Closest Latitude for Coord %d was: %f\n", i,co_ordinates[i].closest_Lat);
            printf("Closest Longitude for Coord %d was: %f\n", i,co_ordinates[i].closest_Long);
            printf("Closest Position ID for Coord %d was: %d\n\n",i, co_ordinates[i].closest_Position_ID);

    }

    end = clock();
    duration = duration + ((double)end-start)/CLOCKS_PER_SEC;
    printf("Execution time: %f\n", duration);

    //Close the "VehiclePositions.dat" data file
    fclose(VehiclePositions);

    return 0;
}

//Converts array of BYTES to Int32_t
int32_t str2Int32_t(uint8_t *str){
    //Local return value
    int32_t out;

    //Convert to int32_t with correct endianness
    out =(str[3] << 24) + (str[2] << 16) + (str[1] << 8) + str[0];


    return out;
}

//Converts array of BYTES to Uint64_t
uint64_t str2Uint64_t(uint8_t *str){
    //Local return value
    uint64_t out;

    //Convert to int32_t with correct endianness
    out = ((uint64_t) str[7] << 56) +((uint64_t) str[6] << 48) +((uint64_t) str[5] << 40) +((uint64_t) str[4] << 32) +  ((uint64_t) str[3] << 24) + ((uint64_t) str[2] << 16) + ((uint64_t) str[1] << 8) + str[0];


    return out;
}


//Read ASCII string of undefined length
//NOTE: this function is the slowest of the read operations and requires improvement
void ASCII_Read(uint8_t *str, FILE *VehiclePositions){

    int i = 0;
    int quit = 0;

    //Read byte by byte until '\0'
    while(quit == 0){
        str[i] = (uint8_t) fgetc(VehiclePositions);
        if(str[i] == 0x00){
            quit = 1;
        }
        i++;
    }

}

//Swap the bytes of a 4 byte array
void byteSwap(uint8_t *str){

    uint8_t hold[4];

    hold[0] = str[3];
    hold[1] = str[2];
    hold[2] = str[1];
    hold[3] = str[0];

    str[0] = hold[0];
    str[1] = hold[1];
    str[2] = hold[2];
    str[3] = hold[3];
}

//Converts an array of BYTES to a float
float str2Float(unsigned char *str) {
    float out;
    memcpy(&out,str,sizeof(out));
    return out;
}
   
