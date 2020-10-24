#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "dStructs.h"
#include "listFun.h"
#include "helper.h"
#include "environment.h"
elevator selectActionQ(elevator e, hyper h, int t);
elevator selectActionSarsa(elevator e, hyper h, int t);
elevator updateQ(elevator e, hyper h, int stateID);
elevator updateSarsa(elevator e, hyper h, int stateID);
#include "elevator.h"
#include "RLalgorithms.h"
#include "statistics.h"
