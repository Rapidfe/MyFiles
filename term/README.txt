임베디드 시스템 프로그래밍 프로젝트 =========

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