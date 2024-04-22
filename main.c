#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "housedefines.h"

#define		SET_POINT				22.0
#define		MAX_FLUCTUATION			2.7778
#define		HEATER_TEMP				50
#define		COST_RATE				(0.09/3.6e6)

#define		DATA_CSV_FILE_RAW		"./data/temp_vasteras_hours.csv"
#define		DATA_CSV_FILE_CLEAN		"./data/temperatures_clean.csv"
#define		DATA_MAX_LENGTH			131391

#define		RUNNING_HOURS			131391

/* structs */
typedef struct {
	double room_temp;
	bool heater_on;
	double out_temp;
} World;

/* function declarations */
static double calculate_heat_emission(World *w);
static double calculate_heat_loss(World *w);
static void print_world_status(World *w);
static double run_heater(World *w);
static double *read_data(void);
static void toggle_heater_power(World *w, bool new_status);
static void update_room_temp(World *w, double heat_gain, double heat_loss);

/* function implementations */
double
calculate_heat_emission(World *w)
{
	/* heat generated from the heater */
	return  w->heater_on * ((HEATER_TEMP - w->room_temp) * C * MDOT);
}

double
calculate_heat_loss(World *w)
{
	/* heat loss from the environment */
	return (w->room_temp - w->out_temp) / R_EQ;
}

void
print_world_status(World *w)
{
	printf("Outer temperature\t\t %0.1f°\n", w->out_temp);
	printf("Current room temperature:\t %.3f°\n", w->room_temp);
	printf("Heater status:\t\t\t %d\n\n", w->heater_on);
}

bool
run_thermostat(World *w)
{
	/** 
	 * if room temperature has not fluctuated above the `SET_POINT` by more
	 * than `MAX_FLUCTUATION`
	 */
	if ((SET_POINT - w->room_temp) > MAX_FLUCTUATION) {
		return true;  // signal to heater to turn on
	}
	/**
	 * if room temperature has not fluctuated bellow the `SET_POINT` by more
	 * than `MAX_FLUCTUATION`
	 */
	else if ((SET_POINT - w->room_temp) < -1*MAX_FLUCTUATION) {
		return false;  /* signal to heater to turn off */
	}

	/* signal to heater to remain as it is */
	return w->heater_on;
}

double *
read_data(void)
{
	FILE *fp;
	char *line = NULL;
    size_t len = 0;
    ssize_t read;
	static double data[RUNNING_HOURS] = {0};
	/* or dynamically */
	/* double *data = calloc(DATA_MAX_LENGTH, sizeof(double)); */

    fp = fopen(DATA_CSV_FILE_CLEAN, "rb");
    if (!fp) {
      fprintf(stderr, "Failed to open %s: %s\n", DATA_CSV_FILE_CLEAN, strerror(errno));
    }

	/* converting data points from strings to double */
	for (size_t i = 0; ((read = getline(&line, &len, fp)) != -1) 
			&& i < RUNNING_HOURS; ++i) {
		data[i] = atof(line);
	}

	/* releasing the memory allocated for the reading the data file */
	free(line);
	fclose(fp);

	return data;
}

void
toggle_heater_power(World *w, bool new_status)
{
	w->heater_on = new_status;
}

void
update_room_temp(World *w, double heat_gain, double heat_loss)
{
	w->room_temp += (heat_gain - heat_loss) / (M * C);
}

int
main(void)
{
	/* initializing the world */
	World w = {
		.room_temp = 20.0,
		.heater_on = false,
	};

	/* reading the temperature data */
	double *data = read_data();

	size_t s = 0, h = 0;  /* seconds (epochs) and hours */
	double gain, loss;
	double cost = 0;
	while (h < RUNNING_HOURS) {
		/* updating the outer temperature every hour */ 
		if (s % 3600 == 0) {
			w.out_temp = data[h];
			h++;
			/* per hour sleep */
			/* sleep(1); */
		}

		/* thermostat signaling to the heater */
		toggle_heater_power(&w, run_thermostat(&w));

		/* calculating heat emission and loss */
		gain = calculate_heat_emission(&w);
		loss = calculate_heat_loss(&w);
		cost += gain * COST_RATE;

		/* updating room temperature */
		update_room_temp(&w, gain, loss);

		/* printing the status */
		// printf("Epoch %d (Hour %d):\n", s, h);
		// printf("Heat emission:\t\t\t %.3f\n", gain);
		// printf("Heat loss:\t\t\t %.3f\n", loss);
		// print_world_status(&w);

		/* per second (epoch) sleep */
		/* sleep(1); */
		s++;
	}

	printf("Epoch %d (Hour %d):\n", s, h);
	printf("---------------------------------------------\n");
	printf("Final temperaute:\t %0.3f°\n", w.room_temp);
	printf("Total cost:\t\t %0.2f$\n", cost);

	/* if `data` is allocated dynamically: */
	/* free(data); */
	return 0;
}
