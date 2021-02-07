#include <stdio.h>

float power_usage(int qty, float powerCost)
{
    float totalCost = 0.0f;

    totalCost = qty * powerCost;

    return totalCost;
}

int main(int argc, char** argv)
{
    int machineA = 15;
    float mA_powerCost = 700.0;

    int machineB = 9;
    float mB_powerCost = 900.0;

    float total_costs[2];

    for (int i = 0; i < 2, i++)
    {
        if (i == 0)
        {
            total_costs[0] = power_usage(machineA, mA_powerCost);
        }
        else if (i == 1)
        {
            total_costs[1] = power_usage(machineB, mB_powerCost);
        }
    }

    printf("Total Power cost for machines in factory = %f\n", total_costs[0] + total_costs[1]);
}