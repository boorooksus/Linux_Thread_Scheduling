# Thread_Scheduling_OperatingSystemConcept2020

### 리눅스 환경에서 pthread와 POSIX semaphore를 이용한 Thread Scheducling과 Multi-Level Queue 구현

#### 프로그램 설명
 본 프로그램은 Ubuntu (64-bit) 운영 체제에서 c언어로 구현하고 실행하였으며 gcc 컴파일러를 이용하여 컴파일 하였습니다.<br>
 여러 개의 프로세스 정보를 입력 받았을 때, 프로세서들을 multi-level queue에 할당하고, 각 queue에서는 각각 SJF(Shortest Job First), 우선순위(Priority), Round-robin, Priority-Round-Robin 기법으로 프로세스들을 스케줄링 한 뒤 순서에 맞춰 프로세스를 수행합니다. 그리고 multi-level queue 사이에는 우선순위가 존재하여 우선 순위 순서에 따라 queue가 실행됩니다.<br>
__세부사항은 보고서(Report.docx)를 참조하십시오.__

#### 강의명
인하대학교 2020년 1학기 오퍼레이팅시스템

***




