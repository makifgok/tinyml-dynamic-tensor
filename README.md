# TinyML: Gömülü Sistemler İçin Dinamik Tensör ve Quantization Yönetimi

Bu depo, RAM kapasitesi KB (Kilobyte) seviyelerinde olan kısıtlı mikrodenetleyiciler (Arduino, ESP32 vb.) üzerinde yapay zeka (TinyML) modellerinin çalıştırılabilmesi amacıyla C dilinde geliştirilmiş bellek dostu bir Tensör mimarisini içermektedir.

## 📌 Proje Amacı ve Mimari Yaklaşım

Standart makine öğrenmesi modelleri 32-bit (FP32) kayan noktalı sayılar kullanır. Gömülü sistemlerde bu durum hızlıca bellek taşmalarına (Out of Memory) yol açar. Bu projede, yüksek seviyeli dillerdeki dinamik matris yapılarının C dilindeki en ilkel ve donanıma yakın hali tasarlanmıştır.

### Teknik Kararlar ve Uygulama:
* **Union ile Bellek Paylaşımı:** Sistemde aynı anda hem `FP32`, hem `FP16` hem de `INT8` dizileri için bellek tahsis edilmez. Tasarlanan `TensorData` union yapısı sayesinde tüm veri tipleri **aynı başlangıç adresini** paylaşır. Böylece sadece o an aktif olan veri tipinin boyutu kadar RAM tüketilir.
* **Ardışık (Contiguous) Bellek Yönetimi:** Dinamik olarak oluşturulan matris elemanları belleğe ardışık olarak dizilir. Bu sayede `void* raw` pointer'ı kullanılarak içerideki veri tipinden bağımsız, sızıntısız (memory leak) bir `free()` işlemi gerçekleştirilir.
* **Quantization (Nicemleme) ve Type Casting:** Ağır FP32 matrisleri, `scale` ve `zero_point` hesaplamaları kullanılarak %75 bellek tasarrufu ile INT8 formatına sıkıştırılır. Dönüşüm sırasında donanım seviyesinde taşmaları önlemek için manuel sınırlandırma (clipping) ve `(int8_t)` tip dönüşümü (type casting) uygulanmıştır.

## 🤖 Agentic Kodlama Süreci

Geliştirme süreci geleneksel kodlamadan farklı olarak "Agentic Kodlama" yaklaşımıyla yürütülmüştür. Gemini dil modeline doğrudan kod yazdırmak yerine; kullanılacak C89 derleyici standartları, bellek kısıtlamaları, union tabanlı mimari gereksinimleri adım adım promptlanarak (yönlendirilerek) iteratif bir geliştirme süreci izlenmiştir.

## 🛠️ Kurulum ve Çalıştırma

Proje, eski nesil derleyicilerle de tam uyumlu çalışabilmesi için standart C (C89/C90) kurallarına göre optimize edilmiştir. 

Lokalinizde derleyip test etmek için:

```bash
git clone [https://github.com/](https://github.com/)[GitHub-Kullanici-Adin]/[Repo-Adin].git
cd [Repo-Adin]
gcc main.c -o tensor_demo
./tensor_demo
