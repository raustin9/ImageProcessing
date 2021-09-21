#include "image.h"
#include <iostream>
#include <sstream>
using namespace std;

char PrintMenu();

int main(int argc, char** argv)
{
    /* Declare variables */
    char option; // Determines what choice to go with in main function
    char buffer[256] = {0};
    string message = ""; // Messge to encode into an image
    string filename;
    string file_extention;

    size_t len = 0;

    while (option != 'Q') {
        option = PrintMenu();

        if (option == 'E') {
            /* Encode Message */
            cin.clear();
            printf("\nChosen: Encoding message into image\n");
            cin.ignore();
            printf("Enter message to encode: > ");
            getline(cin, message);
            printf("The message you have entered is < %s >\n", message.c_str());

            /* Find image to encode */
            printf("Name of image to encode > images/");
            cin >> filename;
            filename = "images/" + filename;

            /* Encode the message into image */
            Image to_encode(filename.c_str());
            to_encode.encodeMessage(message);
            
            /* Write the image */
            printf("Name the new file: encoded_images/");
            cin >> filename;
            filename = "encoded_images/" + filename;
            printf("Writing Image\n\n");
            to_encode.write(filename.c_str());
            /**/
        } else if (option == 'D') {
            /* Decode Message */
            cin.clear();
            printf("Chosen: Decoded message from image\n");
            cin.ignore();

            /* Read in name of file to decode */
            printf("Name of file to decode > ");
            cin >> filename;
            filename = "encoded_images/" + filename;

            /* Declare image that we want to decode */
            Image to_decode(filename.c_str());
            to_decode.decodeMessage(buffer, &len);

            /* Print out the decoded message */
            printf("\nDecoded Message: < %s >\n\n", buffer);
            /**/
        } else if (option == '1') {
            // diff option
            printf("Currently unsupported :(\n");
        } else if (option == '2') {
            // diff scale option
            printf("Currently unsupported :(\n");
        } else if (option == 'G') {
            /* Grayscale Average */
            printf("CHOSEN: Grayscale image with avg\n");
            
            printf("Name of image to grayscale > images/");
            cin >> filename;
            filename = "images/" + filename;

            /* Grayscale the image */
            Image to_grayscale_avg(filename.c_str());
            to_grayscale_avg.grayscale_avg();

            /* Write to new file */
            printf("Name of new file > gray_images/avg/");
            cin >> filename;
            filename = "gray_images/avg/" + filename;
            to_grayscale_avg.write(filename.c_str());

            printf("New image in gray_images/avg\n");
            /**/
        } else if (option == '3') {
            /* Grayscale Lum */
            printf("CHOSEN: Grayscale image with lum\n");
            
            printf("Name of image to grayscale > images/");
            cin >> filename;
            filename = "images/" + filename;

            /* Grayscale the image */
            Image to_grayscale_lum(filename.c_str());
            to_grayscale_lum.grayscale_lum();

            /* Write to new file */
            printf("Name of new file > gray_images/lum/");
            cin >> filename;
            filename = "gray_images/lum/" + filename;
            to_grayscale_lum.write(filename.c_str());

            printf("New image in gray_images/avg\n");
            /**/
        } else if (option == 'C') {
            /* Colormap */
            float r, g, b;

            printf("CHOSEN: Colormask Image\n");
            printf("Name of image to colormask > images/");
            cin >> filename;
            filename = "images/" + filename;

            /* Declare image to colormap */
            Image to_colormask(filename.c_str());

            /* Get values to colormap */
            printf("[0,1] - Red   > ");
            cin >> r;
            printf("[0,1] - Green > ");
            cin >> g;
            printf("[0,1] - Blue  > ");
            cin >> b;

            /* Write the image */
            printf("Name of new file > colormask_iamges/");
            cin >> filename;
            filename = "colormask_images/" + filename;
            to_colormask.colorMask(r,g,b);
            to_colormask.write(filename.c_str());
            printf("New image in colormask_iamges\n");
            /**/
        }
    }
    return 0;
}

/* Function to print the menu of options for the main function and then return the selected one */
char PrintMenu() {
    string option;

    printf("\nOptions: \n * E  - Encode message\n * D  - Decode message\n * DI - Diffmap two images\n * DS - Diffmap scale two images\n * GA - Grayscale Image\n * GL - Grayscale Lum\n * C  - Colormask Image\n * Q  - Quit program\n\nChoose Option > ");
    cin >> option;

    if (option == "E" || option == "e") {
        // Option E
        return 'E';
    } else if (option == "D" || option == "d") {
        // Option D
        return 'D';
    } else if (option == "DI" || option == "di") {
        // Option DI
        return '1';
    } else if (option == "DS" || option == "ds") {
        // Option DS
        return '2';
    } else if (option == "Q" || option == "q") {
        // Option Q
        return 'Q';
    } else if (option == "GA" || option == "ga") {
        // Option Q
        return 'G';
    } else if (option == "GL" || option == "gl") {
        // Option Q
        return '3';
    } else if (option == "C" || option == "c") {
        // Option Q
        return 'C';
    } else {
        // Invalid choice of input
        printf("Invalid input. Choose again\n");
    }
    return '0';
}