WRMASSMAX = 5000.0
WRMASSMIN = 1000.0
WRMASSINT = 100.0
NRMASSMAX = 1000.0
NRMASSMIN = 100.0
NRMASSINT = 50.0



def my_range(start, end, step):
    while start <= end:
        yield start
        start += step

WRmassList = [x for x in my_range(WRMASSMIN, WRMASSMAX, WRMASSINT)]
massMatrix = {}
for WRmass in WRmassList:
    massMatrix[WRmass] = [x for x in my_range(NRMASSMIN,NRMASSMAX if  NRMASSMAX < (WRmass/3.0) else WRmass/3.0,NRMASSINT)]

for WRmass in WRmassList:
    for NRmass in massMatrix[WRmass]:
        print str(WRmass)+" "+str(NRmass)
