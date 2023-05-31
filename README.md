# ElevatorSimulator-XHyper320TKU
**Project for Microprocessor Application Circuit Design**

> 본 프로젝트는 마이크로프로세서 응용회로 설계실습의 기말프로젝트로, Hybus사의 교육용 임베디드 장비 XHyper320TKU 위에서 작동하는 프로그램입니다.

## Introduction
XHyper320TKU는 임베디드 장비로, 이 장비가 가지는 제약 조건이 엘리베이터와 유사하며, 엘리베이터는 실생활에서 임베디드 장비가 사용되는 예시 중 하나입니다.

또한, 엘리베이터는 임베디드 장치가 어떻게 작동하고 어떻게 사람들의 삶을 개선할 수 있는지 보여주고 있습니다.

위와 같은 이유로 임베디드 장비로 실습을 하기에 좋은 주제라고 생각하여 엘리베이터 시뮬레이터를 프로젝트 주제로 선정하였습니다.

input과 elevator의 로직을 분리한 후, **Multi-Thread를 사용하여 엘리베이터가 움직이는 동안 입력한 층으로도 이동 가능하도록 제작**하였습니다.

이를 위해 두 로직이 공유하는 데이터에 접근 시 오류가 발생하지 않도록 input 로직과 elevator 로직 사용시 mutex를 사용하였습니다.

최대한 실제 엘리베이터와 비슷하게 동작하도록 구현하였습니다. XHyper320TKU 장비의 키패드의 특성으로 인해 누른 층수의 기능은 구현하지 않았습니다.


## Build

### Build for XHyper320TKU
```
$ make
```

### Build for Ximulator
```
$ make xim
```

### Clean build
```
$ make clean
```

## Execution

### Execution for XHyper320TKU
```shell
$ ./elevator {elevator_floor} {max_floor}
```

### Execution for Ximulator
```shell
$ ./elevator_xim {elevator_floor} {max_floor}
```

- `./elevator`, `./elevator_xim` : 프로그램 실행
- `elevator_floor`(optional): 시작 층수 (1층 이상 최대 층수 이하)
- `max_floor`(optional): 최대 층수 (14층 이하)
