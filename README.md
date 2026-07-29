# LS Mk.1 Drone Launcher
Compact drone launcher + central compute module

## 3D Model Preview
![3D PCB](model2.png)

## 3D PCB Preview
![3D PCB](model.png)

## Wiring
![PCB](wiring.png)

## Schematic
![Schematic](schematic.png)

### Download ./latestProduction to begin fabricating the PCB

## Introduction
The LS Mk.1 Drone Launcher is a 3D printed platform and main control unit for the LS Mk.1 drones. It has a capacity of 5 drones each, stacked on top of each other to maximize space. The main control unit consists of a raspbbery pi zero 2W with a custom wifi and battery hat. The hat provides better wifi range and a battery time of more than an hour.


## Bill of Materials 

### (Minimum build for testing: 5 drones)

| Quantity | Components | Price | Link |
|--------- |----------|----------| -----|
| 1  | Raspberry Pi Zero 2 W | ~15$ | [here](https://www.sparkfun.com/raspberry-pi-zero-2-w.html?src=raspberrypi)
| 1 | Clear PLA Filament | ~20$ | [here](https://www.amazon.com/SUNLU-Filament-Transparent-Dimensional-Accuracy/dp/B07ZNG4L9P?channelId=520&clpRedir=Y&dib=eyJ2IjoiMSJ9._Y4kmvLmwjvktiYmXwsyz_Am5xx7NReuKA_k9fHzIejFf9UoUlhPqIUR2Kfcu_mx67XBB3snagQ-fa1A_Ys3FWvcWoi1dI5MsJ6hGMeDpd1BIR1tq_-gIXaTG5BrTvI7VzTtiaEztBTeRebkZxvTPw4tFbMlbzCpfKi1du8ktHhgZnWP9pK3flO77HIb2ggK4b0B9uJqkWQFQodsriMgJxH84ljwj4H8OITJnoYh_vE.2zkivrb0O9D5yNAwScLuwbLs3Lk4bP8leoBIEJbwXRQ&dib_tag=se&keywords=transparent%2Bpla&plpRedirect=mhFallback&qid=1785305371&sr=8-4&th=1)
| 1 | 1200mAh Lipo Battery | ~10$ [here](https://www.aliexpress.us/item/3256811414820649.html?spm=a2g0o.productlist.main.1.46bdwH3SwH3Sbj&algo_pvid=cdb969a2-c563-41ce-bc78-12d1dc257c78&algo_exp_id=cdb969a2-c563-41ce-bc78-12d1dc257c78-0&pdp_ext_f=%7B%22order%22%3A%2296%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21USD%2111.31%216.16%21%21%2176.16%2141.49%21%402101d3fe17841520345512246e1288%2112000056027143357%21sea%21US%217493938711%21X%211%210%21n_tag%3A-29919%3Bd%3Ad260408f%3Bm03_new_user%3A-29895%3BpisId%3A5000000211679188&curPageLogUid=uhXJ9YqKgNaO&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005011601135401%7C_p_origin_prod%3A)
| 1 | 155mm Leather Handle | ~2$ | [here](https://www.aliexpress.us/item/3256812155687340.html?spm=a2g0o.productlist.main.15.27dajTuqjTuqFp&algo_pvid=e87958b2-7cc6-45ee-9165-c5cc9b5004c7&algo_exp_id=e87958b2-7cc6-45ee-9165-c5cc9b5004c7-14&pdp_ext_f=%7B%22order%22%3A%221%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21USD%211.72%211.02%21%21%2111.56%216.84%21%402101e5ab17852684858322898e0dcb%2112000058138770576%21sea%21US%210%21ABX%211%210%21n_tag%3A-29910%3Bd%3Ad260408f%3Bm03_new_user%3A-29895%3BpisId%3A5000000210913316&curPageLogUid=BP5HyM53Apva&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005012342002092%7C_p_origin_prod%3A)

| 4 | 200mm x 4mm Metal Rods | $5 | [here](https://www.aliexpress.us/item/3256805290944526.html?spm=a2g0o.productlist.main.3.2df5s35ms35mOY&algo_pvid=af55ae4c-899e-4dcc-919f-7cc0f6e58495&algo_exp_id=af55ae4c-899e-4dcc-919f-7cc0f6e58495-2&pdp_ext_f=%7B%22order%22%3A%224031%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21USD%212.22%212.15%21%21%2114.95%2114.51%21%402101de2517853088871028484e0d56%2112000033239843707%21sea%21US%217493938711%21X%211%210%21n_tag%3A-29919%3Bd%3Ad260408f%3Bm03_new_user%3A-29895&curPageLogUid=mLtoWIDfhah6&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005005477259278%7C_p_origin_prod%3A)
| 1  | RPI Hat PCB | ~15$ | Download repo, extract fabrication files, and fabricate through JLCPCB (complete instructions for this may come soon)
| 1  | Electronic Components | ~30$ | See production BOM for a full list, the PCB fabrication files automatically include these components as well.

Total: $97

----

## Tools / Other parts:
- Hot air gun or hot plate
- Solder paste
- Tweazers

## Assembly
- Apply solder paste to all the solder pads
- Use tweazers to place components and reflow with hot air gun or hot plate
- Assemble control unit case and mount battery and antenna onto the pcb and raspbbery pi zero 2W
