$fn = 64;
THICKNESS = 3;
RADIUS = 22;
SEGMENT_LENGTH = 0.4;// 0.1
SCALE_FACTOR = SEGMENT_LENGTH/3;
LENGTH = 160;
SEGMENTS = LENGTH / SEGMENT_LENGTH;



module housing(){
    union(){
        for(i = [0:SEGMENTS]){
            translate([0,-((i*SCALE_FACTOR)^2)/100,SEGMENT_LENGTH * i]) rotate([i*SCALE_FACTOR/4,0,0]) {
                if(i < SEGMENTS * 0.85){
                    difference(){
                        cylinder(SEGMENT_LENGTH, RADIUS, RADIUS);
                        cylinder(SEGMENT_LENGTH, RADIUS-THICKNESS, RADIUS-THICKNESS);
                    }
                } else if(i < SEGMENTS){
                    s = (i - (SEGMENTS * 0.85))/(SEGMENTS * 0.1);
                    scale([1-s/3.5,1 - s/3,1]) difference(){
                        cylinder(SEGMENT_LENGTH, RADIUS-sqrt(1 * 200) + sqrt((s+1)*200), RADIUS-sqrt(1 * 200) + sqrt((s+1)*200));
                        cylinder(SEGMENT_LENGTH, RADIUS-THICKNESS, RADIUS-THICKNESS);
                    }
                } else {
                    s = (i - (SEGMENTS * 0.85))/(SEGMENTS * 0.1);
                    scale([1-s/3.5,1 - s/3,0.3]) difference(){
                        sphere(RADIUS-sqrt(1 * 200) + sqrt((s+1)*200));
                        sphere(RADIUS-THICKNESS);
                        translate([-RADIUS, -RADIUS, -RADIUS * 5]) cube([RADIUS * 2, RADIUS * 2, RADIUS*5]);
                        
                    }
                }
                
            }
        }
    }
}

housing();

OFFSET = 1;
module housing_plug(){
    difference(){
        union(){
            scale([1,1,0.5]) difference(){
                sphere(RADIUS);
                sphere(RADIUS-THICKNESS);
                translate([-RADIUS, -RADIUS, 0]) cube([RADIUS * 2, RADIUS * 2, RADIUS]);
            }
            translate([0,0, -5]) difference(){
                cylinder(15, RADIUS-THICKNESS-OFFSET, RADIUS-THICKNESS-OFFSET);
                cylinder(15, RADIUS-THICKNESS*2-OFFSET, RADIUS-THICKNESS*2-OFFSET);
            }
        }
        translate([-6,0,-20]) cylinder(50,7.7,7.7);
        translate([5.2,-7,-20]) cube([8,14,20]);
    }
}
//housing_plug();
/*
difference(){
    translate([0,0,0]) housing();
    translate([-100,-100,80]) cube([200,200,100]); 
}*/

MOTOR_WIDTH = 15;
MOTOR_LENGTH = 30;
MOTOR_HEIGHT = 12;
MOTOR_OFFSET = 0.2;
MOTOR_PADDING = 4;
VIBE_LENGTH = 12;
VIBE_EXTRA = 2;
module motor_housing(){
    //translate([0,0,-5]) cube([10,10,5]);
    difference(){
        cube([MOTOR_WIDTH+MOTOR_OFFSET*2 + MOTOR_PADDING, MOTOR_LENGTH+MOTOR_OFFSET*2, MOTOR_HEIGHT + MOTOR_OFFSET*2 + MOTOR_PADDING]);
        translate([MOTOR_PADDING/2,0, MOTOR_PADDING/2]) cube([MOTOR_WIDTH+MOTOR_OFFSET*2, MOTOR_LENGTH+MOTOR_OFFSET*2, MOTOR_HEIGHT + MOTOR_OFFSET*2]);
        translate([MOTOR_PADDING/2 - VIBE_EXTRA/2,0, MOTOR_PADDING/2 - VIBE_EXTRA/2]) cube([MOTOR_WIDTH+MOTOR_OFFSET*2 + VIBE_EXTRA, VIBE_LENGTH, MOTOR_HEIGHT + MOTOR_OFFSET*2 + VIBE_EXTRA]);
    }
}

//motor_housing();
