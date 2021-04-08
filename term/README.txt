임베디드 시스템 프로그래밍 term project =========
				5조 박민철
				     이지윤
본 프로그램 실행에 필요한 단계입니다.

1.
git clone https://github.com/WiringPi/WiringPi.git
cd WiringPi
git pull origin
./build
(gpio -v : 버전 확인)
(gpio readall : in out port 확인)
(gpio mode 1 out : 1번 포트 out으로 설정)

2. 
make all

3.
이미지 폴더 'image' -> /var/www/html/에 옮기기.

3.
./host [host서버 IP]
./client [host서버 IP]

컴파일 문제가 있을시 연락 부탁드립니다.
01093654981