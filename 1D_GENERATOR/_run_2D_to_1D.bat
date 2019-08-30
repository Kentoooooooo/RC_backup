cd /d %~dp0

for %%A in (*.csv) do (
    If not exist %date:~0,4%%date:~5,2%%date:~8,2%%%A1D_data mkdir %date:~0,4%%date:~5,2%%date:~8,2%%%A1D_data
    move %%A %date:~0,4%%date:~5,2%%date:~8,2%%%A1D_data
    move gen_1D.py %date:~0,4%%date:~5,2%%date:~8,2%%%A1D_data
    cd %date:~0,4%%date:~5,2%%date:~8,2%%%A1D_data
    python gen_1D.py %%A
    move gen_1D.py ..
    If not exist x_data mkdir x_data
    If not exist y_data mkdir y_data
    move x*.csv x_data
    move y*.csv y_data
    move %%A ..
    cd ..
)

