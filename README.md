## Kullanılan socket fonksiyonları

* ```int socket(int domain, int type, int protocol);```

  - int domain: Soketin etki alanı - Oluşturulacak soketin etki alanını belirtir.
  Örneğin, IPv4 adreslerini kullanmak için AF_INET veya IPv6 adreslerini kullanmak için AF_INET6 gibi değerler kullanılabilir.
  - int type: Soketin tipi - Oluşturulacak soketin tipini belirtir. Örneğin, TCP (Stream) soketler için SOCK_STREAM veya UDP (Datagram) soketler için SOCK_DGRAM gibi değerler kullanılabilir.
  - int protocol: Soketin protokolü - Oluşturulacak soketin kullanacağı protokolü belirtir. Genellikle 0 değeri kullanılarak, domain ve tip parametrelerine bağlı olarak uygun bir protokol otomatik olarak seçilir.
  socket() fonksiyonu, başarılı bir şekilde tamamlandığında yeni bir soketin tanımlayıcısını (socket descriptor) döndürür. Hata durumunda -1 döndürerek bir hata kodu ile fonksiyonun tamamlandığını belirtir.

* ```int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);```

  - sockfd: Soketin tanımlayıcısı (socket descriptor) - Değiştirilecek soketin tanımlayıcısıdır.
  - int level: Seçeneğin seviyesi - Ayarlanmak istenen seçeneğin seviyesini belirtir.
    Genellikle SOL_SOCKET seviyesi kullanılır, soket seviyesindeki seçenekleri temsil eder.
    Diğer seviyeler, protokol seviyelerindeki (örneğin, TCP veya UDP) seçenekleri değiştirmek için kullanılabilir.
  - int optname: Seçeneğin adı - Ayarlanmak istenen seçeneğin adını belirtir.
    Örnek olarak, SO_REUSEADDR gibi bir soket seçeneği adı kullanılabilir.
  - const void *optval: Seçeneğin değeri - Ayarlanmak istenen seçeneğin değerini belirtir.
    Bu parametre için bir işaretçi kullanılır.
  - socklen_t optlen: Seçeneğin değerinin boyutu - optval parametresinin işaret ettiği bellek alanının boyutunu belirtir.

* ```int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);```

  - sockfd: Bağlanacak soketin dosya tanımlayıcısı (file descriptor).
  - addr: Bağlanacak adresin yapısını temsil eden bir struct sockaddr işaretçisi. IPv4 adresleri için struct sockaddr_in yapısı, IPv6 adresleri için struct sockaddr_in6 yapısı kullanılır.
  - addrlen: addr yapıının boyutunu temsil eden bir socklen_t değeri.
  
    ```markdown
    bind() fonksiyonu, bir soketi belirtilen IP adresi ve port numarasına bağlar.
    IP adresi, genellikle struct sockaddr_in veya struct sockaddr_in6 yapılarındaki sin_addr veya sin6_addr alanına atanırken,
    port numarası da sin_port veya sin6_port alanına atanır.

* ```int listen(int sockfd, int backlog);```

  - sockfd: Bağlantıları dinlenecek soketin dosya tanımlayıcısı (file descriptor).
  - backlog: Kuyruğa alınacak en fazla bağlantı sayısını temsil eden bir tam sayı değeri.
  
    ```markdown
    listen() fonksiyonu, belirtilen soketi pasif modda dinlemeye başlar.
    backlog parametresi, dinlenecek bağlantıların kuyruğa alınabileceği maksimum sayıyı belirler.
    Eğer bu kuyruk dolarsa, yeni bağlantılar reddedilir.

* ```int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);```

  - sockfd: Bağlantı taleplerini kabul edecek olan soketin dosya tanımlayıcısı (file descriptor).
  - addr: Yeni bağlantının adres bilgisini içeren bir struct sockaddr işaretçisi. İstemci bağlantısı bilgileri bu yapıya yazılır.
  - addrlen: addr yapısının boyutunu temsil eden bir socklen_t işaretçisi.
    Başlangıçta, bu değişkenin değeri addr yapısının boyutunu içermelidir.
    accept() fonksiyonu tarafından gerçekleştirilen bağlantı ile ilgili adres bilgisi bu değişkene yazılır.
  
    ```markdown
    accept() fonksiyonu, listen() fonksiyonu ile belirtilen soketi dinler ve bir istemci bağlantısı kabul eder.
    Yeni bir soket oluşturarak istemci ile iletişim kurmak için kullanılır.
    addr ve addrlen parametreleri, kabul edilen istemci bağlantısının adres bilgilerini içerir.

* ```ssize_t recv(int sockfd, void *buf, size_t len, int flags);```

  - sockfd: Veri alınacak soketin dosya tanımlayıcısı (file descriptor).
  - buf: Alınan verinin okunacağı bellek tamponunun işaretçisi.
  - len: Okunmak istenen verinin maksimum uzunluğunu temsil eden bir tam sayı değeri.
  - flags: İsteğe bağlı bayraklar (flags) parametresi.
    Bu parametre, veri alımı sırasında farklı davranışlar belirlemek için kullanılabilir. Genellikle 0 olarak belirtilir.
  
    ```markdown
    recv() fonksiyonu, belirtilen soketten veri okuyarak buf işaretçisine kopyalar.
    len parametresi, okunacak maksimum veri uzunluğunu belirler.
    Fonksiyon, gerçekten okunan veri uzunluğunu döndürür. Okunan veri yoksa 0 döndürür.

* ```int poll(struct pollfd *fds, nfds_t nfds, int timeout);```

  - fds: struct pollfd türündeki bir dizi, her bir soketin durumunu ve dinlenecek olayları temsil eden yapıları içerir.
  - nfds: fds dizisinin eleman sayısı.
  - timeout: Fonksiyonun bloklanma süresini belirten bir zaman aşımı değeri.
    timeout değeri negatif bir sayı ise poll() fonksiyonu bloklu olarak çalışır ve olay meydana gelene kadar bekler.
    timeout değeri 0 ise poll() fonksiyonu hemen döner ve olayların durumunu kontrol eder.

      struct pollfd yapısı, poll() fonksiyonunda kullanılan her bir soketin durumunu ve dinlenecek olayları temsil eder. Bu yapı aşağıdaki gibidir:
      ```C++
      struct pollfd {
          int fd;
          short events;
          short revents;
      };

    - fd: Soketin dosya tanımlayıcısı (file descriptor).
    - events: Dinlenen olayları belirten flags.
        Hangi olayların dinleneceği bu alanda belirtilir. Örneğin POLLIN okuma olayını, POLLOUT yazma olayını  temsil eder.
    - revents: Geri dönen olayları (returned events) belirten bayraklar (flags).
        Bu alan, poll() fonksiyonu tarafından geri döndürülen olayları temsil eder.
        Hangi olayların  meydana geldiği bu alanda belirtilir.

    poll() fonksiyonu, fds dizisinde belirtilen soketleri dinler ve olayların meydana gelip gelmediğini kontrol eder.
    events alanında belirtilen olayların meydana gelmesi durumunda, revents alanında ilgili bayraklar (flags) ayarlanır.
