#include <stdio.h>

float total_price(int qty, float unit_price)
{
    float t_price = 0.0f;

    t_price = qty * unit_price;

    return t_price;
}

int main(int argc, char** argv)
{
    int apple = 10;
    float apple_price = 500.0;

    int banana = 5;
    float banana_price = 10.50;

    float total_prices[2];

    for (int i = 0; i < 2, i++)
    {
        if (i == 0)
        {
            total_prices[0] = total_price(apple, apple_price);
        }
        else if (i == 1)
        {
            total_prices[1] = total_price(banana, banana_price);
        }
    }

    printf("total price = %f\n", total_prices[0] + total_prices[1]);
}