
idf_component_register(
    SRCS 
    	"main.c"
    	"src/dht11.c"
    	"src/i2c_helper.c"
    	"src/mpu6050.c"
    INCLUDE_DIRS 
    	"."
    	"inc"
)
