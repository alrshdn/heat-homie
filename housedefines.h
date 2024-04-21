/* defining useful values */
#define PI 3.14159265358979323846
#define R2D (180.0 / PI)


/* defining the house geometry */
#define HOUSE_LENGTH 20.0
#define HOUSE_WIDTH 10.0
#define HOUSE_HEIGHT 4.0

#define ROOF_PITCH (40.0 / R2D)
#define NUM_WINDOWS 6.0
#define WINDOW_HEIGHT 1.0
#define WINDOW_WIDTH 1.0

#define WINDOW_AREA (NUM_WINDOWS * WINDOW_HEIGHT * WINDOW_WIDTH)
#define WALL_AREA (2 * HOUSE_LENGTH * HOUSE_HEIGHT \
		+ 2 * HOUSE_WIDTH * HOUSE_HEIGHT \
		+ 2 * (1 / cos(ROOF_PITCH/2)) * HOUSE_WIDTH * HOUSE_LENGTH \
		+ (tan(ROOF_PITCH) * HOUSE_WIDTH) - WINDOW_AREA)


// defining the type of insulation used
// glass wool in the walls, 0.2 m thick
// k is in units of J/sec/m/C
#define K_WALL 0.038
#define L_WALL 0.2
#define R_WALL (L_WALL / (K_WALL * WALL_AREA))

// glass windows, 0.01 m thick
#define K_WINDOW 0.78
#define L_WINDOW 0.01
#define R_WINDOW (L_WINDOW / (K_WINDOW * WINDOW_AREA))

// determining the equivalent thermal resistance for the whole building
#define R_EQ ((R_WALL * R_WINDOW) / (R_WALL + R_WINDOW))

#define C 1005.4
#define MDOT 1.0

#define AIR_DENSITY 1.2250
#define M ((HOUSE_LENGTH * HOUSE_WIDTH * HOUSE_HEIGHT + tan(ROOF_PITCH) * HOUSE_WIDTH * HOUSE_LENGTH) * AIR_DENSITY)

typedef enum {
	false = 0,
	true = 1
} bool;

