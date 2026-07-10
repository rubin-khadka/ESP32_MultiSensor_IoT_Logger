
idf_component_register(
    SRCS 
    	"main.c"
    	"src/dht11.c"
    	"src/i2c_helper.c"
    	"src/mpu6050.c"
    	"src/dht11_task.c"
    	"src/i2c_helper.c"
    	"src/mpu6050_task.c"
    INCLUDE_DIRS 
    	"."
    	"inc"
)
