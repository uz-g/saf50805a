   #pragma once

   #include "main.h"

   namespace robot {
       void initChassis();
       void updateDrive(int leftY, int rightY, bool reverse);
   }