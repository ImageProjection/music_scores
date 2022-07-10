#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>//for fabs

/*
void print_image_file(char* image, int W, int H)
{
    FILE *out_image;
    out_image=fopen("out_image.txt", "w");

    for (int i=0;i<H;i++)
    {
        for (int j=0;j<W;j++)
            fprintf(out_image,"%c",(image[i*W+j]==0 ? '.' : '#' ));
        fprintf(out_image,"\n");
    }

    fclose(out_image);
}
*/

int get_closest_index(float* array, int number)
{
    int min_index=-1;
    float min_distance=10000;//since max H is 300
    for (int i=0;i<12;i++)//there are 12 positions on staff
    {
        if (fabs(number-array[i]) < min_distance)
        {
            min_distance=fabs(number-array[i]);
            min_index=i;
        }
    }
    return min_index;
}

//sum of values in rectangle starting at (row,col) with dimensions w and h
//vertical sum is sum(image,W,H,0,col_index,1,H)
int sum(char* image, int W, int H, int row, int col, int w, int h)
{
    int result=0;
    for (int i=0;i<h;i++)
        for (int j=0;j<w;j++)
            result+=image[(row+i)*W+(col+j)];
    return result;
}

int main()
{
    int W;
    int H;
    scanf("%d%d", &W, &H); 
    fgetc(stdin);
    char* image=malloc(W*H*sizeof(char));
    int color;
    int len;
    int image_index=0;
    //getting image   
    while (true)
    {
        color=fgetc(stdin);
        scanf("%d",&len);
        if (color=='B')
            for (int i=0;i<len;i++)
                image[image_index++]=1;
        else
            for (int i=0;i<len;i++)
                image[image_index++]=0;
        if (fgetc(stdin) != ' ')
            break;
    }
    //print_image_file(image,W,H);

    //finding staff parameters
    int row=0;
    int col=0;
    int line_height=0;
    int line_separation=0;
    int z;//row in which top left black pixel is located
    while (sum(image,W,H,0,col,1,H)==0)//moving right
        col++;
    while(image[row*W+col]==0)//scanning from top
        row++;
    z=row;    
    while(image[row*W+col]==1)
    {
        row++;
        line_height++;
    }
    while(image[row*W+col]==0)
    {
        row++;
        line_separation++;
    }

    //approximate possible note row positions
    char notes[] = "GFEDCBAGFEDC";
    float positions[12];
    positions[0]=z-0.5*line_separation;
    positions[1]=z+0.5*line_height;
    for (int j=2;j<=11;j++)
    {
        positions[j]=positions[j-2]+line_height+line_separation;
    }

    //setting staff pixels to 0
    int corner;//row of upper left corner of each line
    for(int line=0;line<6;line++)
    {
        corner=z+line*(line_height+line_separation);
        //erasing rows
        for (int c=0;c<line_height;c++)
            for(int j=0;j<W;j++)
                image[(corner+c)*W+j]=0;
    }

    //evaluating vertical sums (regions between notes are 0)
    int* vertical_sums=malloc(W*sizeof(int));
    for(int j=0;j<W;j++)
        vertical_sums[j]=sum(image,W,H,0,j,1,H);

    //deciphering score
    int left_border=0;
    int right_border=0;
    int upper_tick, lower_tick, ticks_col;
    float equator;
    int position_index;
    int h_compensation;//if note is on a line, its part was erased
    char type;//'H' or 'Q'
    bool is_first=true;//to avoid trailing space
    bool on_the_line;
    while (true)
    {
        //finding region containing only 1 note
        while( (left_border < W) && (vertical_sums[left_border] == 0) )
            left_border++;
        if (left_border==W)
            break;
        right_border=left_border;
        while( vertical_sums[right_border] != 0 )
            right_border++;
        right_border--;

        //finding the note position
        if (vertical_sums[left_border] < vertical_sums[right_border])
            ticks_col=left_border;
        else
            ticks_col=right_border;
        upper_tick=0;
        lower_tick=H-1;
        while (image[upper_tick*W+ticks_col]==0)
            upper_tick++;
        while (image[lower_tick*W+ticks_col]==0)
            lower_tick--;
        equator=0.5*(upper_tick+lower_tick);        
        position_index=get_closest_index(positions,equator);

        //determining the type
        on_the_line=(bool)(position_index % 2);
        if (on_the_line)
        {
            ticks_col=(left_border+right_border)/2;
            upper_tick=0;
            lower_tick=H-1;
            while (image[upper_tick*W+ticks_col]==0)
                upper_tick++;
            while (image[lower_tick*W+ticks_col]==0)
                lower_tick--;
            if (sum(image,W,H,0,ticks_col,1,H) + line_height < lower_tick-upper_tick)
                type='H';
            else
                type='Q';
            
        }
        else
        {
            if (sum(image,W,H,(int)equator,left_border,right_border-left_border+1,1) < right_border-left_border)
                type='H';
            else
                type='Q';
        }

        //printing note and type        
        if (!is_first)
            printf(" ");
        is_first=false;
        printf("%c%c",notes[position_index],type); 
        left_border=right_border+1;
    }
    printf("\n");
    
    free(image);
    return 0;
}