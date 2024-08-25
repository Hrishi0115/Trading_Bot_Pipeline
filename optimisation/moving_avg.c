#include <stdio.h>

// Function to calculate the sum of an array
double sum_array(double *array, int length) {
    double sum = 0.0;
    for (int i = 0; i < length; i++) {
        sum += array[i];
    }
    return sum;
}

// Function to update the moving average windows and calculate the new averages
void update_moving_averages(double *short_mavg_window, double *long_mavg_window, int short_len, int long_len, double new_price, double *short_mavg, double *long_mavg) {
    double prev_short_sum = sum_array(short_mavg_window, short_len);
    double prev_long_sum = sum_array(long_mavg_window, long_len);

    // Append the new price and remove the oldest price
    for (int i = 1; i < short_len; i++) {
        short_mavg_window[i - 1] = short_mavg_window[i];
    }
    short_mavg_window[short_len - 1] = new_price;

    for (int i = 1; i < long_len; i++) {
        long_mavg_window[i - 1] = long_mavg_window[i];
    }
    long_mavg_window[long_len - 1] = new_price;

    double new_short_sum = prev_short_sum + new_price - short_mavg_window[0];
    double new_long_sum = prev_long_sum + new_price - long_mavg_window[0];

    *short_mavg = new_short_sum / short_len;
    *long_mavg = new_long_sum / long_len;
}

int main() {
    // Example usage
    double short_mavg_window[] = {1.0, 2.0, 3.0, 4.0};
    double long_mavg_window[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    int short_len = 4;
    int long_len = 6;
    double new_price = 5.0;
    double short_mavg, long_mavg;

    update_moving_averages(short_mavg_window, long_mavg_window, short_len, long_len, new_price, &short_mavg, &long_mavg);

    printf("New short moving average: %f\n", short_mavg);
    printf("New long moving average: %f\n", long_mavg);

    return 0;
}

// how to run C in VScode:

// gcc -o moving_avg.exe moving_avg.c
// .\moving_avg.exe
