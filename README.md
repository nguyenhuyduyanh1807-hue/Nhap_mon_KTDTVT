[Nhap_mon_KTDTVT.txt](https://github.com/user-attachments/files/24597644/Nhap_mon_KTDTVT.txt)
/* * DỰ ÁN: MÁY ĐO NHỊP TIM VÀ SPO2 (DTM E-SMART)
 * THIẾT BỊ: Arduino/NodeMCU + MAX30100 + OLED SSD1306
 * -------------------------------------------------------
 * ĐÂY LÀ BẢN CODE GỐC CỦA BẠN VỚI ĐẦY ĐỦ CHÚ THÍCH CHI TIẾT
 */

#include <Wire.h>                // Thư viện giao tiếp I2C (để điều khiển màn hình và cảm biến)
#include <Adafruit_GFX.h>         // Thư viện đồ họa cơ bản của Adafruit
#include <Adafruit_SSD1306.h>     // Thư viện điều khiển màn hình OLED SSD1306
#include <Fonts/FreeSerif9pt7b.h> // Font chữ Serif cỡ 9 để hiển thị đẹp hơn
#include "MAX30100_PulseOximeter.h" // Thư viện xử lý dữ liệu cho cảm biến MAX30100

// Định nghĩa thời gian giãn cách giữa mỗi lần cập nhật màn hình là 1000ms (1 giây)
#define REPORTING_PERIOD_MS     1000

// Khởi tạo đối tượng màn hình OLED với độ phân giải 128x64
// -1 nghĩa là không sử dụng chân Reset cứng
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// Khởi tạo đối tượng pox để quản lý cảm biến nhịp tim
PulseOximeter pox;

// KHAI BÁO CÁC BIẾN QUẢN LÝ THỜI GIAN VÀ DỮ LIỆU
uint32_t tsLastReport = 0; // Lưu mốc thời gian cuối cùng in dữ liệu ra màn hình
uint32_t tsLastBeat = 0;   // Lưu mốc thời gian cuối cùng cảm biến nhận được 1 nhịp tim
float smoothedBPM = 0;     // Biến lưu giá trị nhịp tim đã được làm mượt (tránh nhảy số)

// HÀM CALLBACK: Tự động chạy mỗi khi cảm biến phát hiện một nhịp đập (beat)
void onBeatDetected(){
    tsLastBeat = millis(); // Cập nhật lại mốc thời gian có nhịp tim mới nhất
}

void setup(){
    // Khởi tạo cổng Serial để theo dõi trên máy tính với tốc độ 115200 baud
    Serial.begin(115200);

    // Bắt đầu giao tiếp I2C
    Wire.begin();
    // Đặt tốc độ I2C là 400kHz (tốc độ cao) để dữ liệu từ cảm biến truyền về nhanh hơn
    Wire.setClock(400000);

    // Kiểm tra và khởi động màn hình OLED tại địa chỉ I2C 0x3C
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  
        for(;;); // Nếu không thấy màn hình, dừng chương trình mãi mãi
    }
    
    // --- THIẾT LẬP GIAO DIỆN CHÀO MỪNG (SPLASH SCREEN) ---
    display.setFont(&FreeSerif9pt7b); // Sử dụng font chữ đã chọn
    display.clearDisplay();           // Xóa sạch bộ đệm màn hình
    display.setTextColor(WHITE);      // Đặt màu chữ là trắng (đối với OLED đơn sắc)
    
    display.setCursor(20,15);         // Đặt con trỏ tại tọa độ x=20, y=15
    display.println("Welcom to");
    
    display.setCursor(0,40);          // Đặt con trỏ tại tọa độ x=0, y=40
    display.println("DTM E-SMART");
    
    display.display();                // Đẩy dữ liệu từ bộ đệm ra màn hình vật lý
    delay(2000);                      // Giữ màn hình chào mừng trong 2 giây

    // Khởi động cảm biến MAX30100
    if (!pox.begin()) {
        for(;;); // Nếu cảm biến lỗi (chưa cắm dây/nguồn yếu), dừng chương trình
    }
    
    // Cấu hình dòng điện cho LED hồng ngoại (7.6mA là mức tiêu chuẩn)
    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
    
    // Đăng ký hàm onBeatDetected để gọi mỗi khi có nhịp tim
    pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop(){
    // CẬP NHẬT DỮ LIỆU CẢM BIẾN (Phải gọi hàm này liên tục trong loop)
    pox.update();

    // KIỂM TRA ĐIỀU KIỆN ĐỂ CẬP NHẬT MÀN HÌNH (Sau mỗi 1 giây)
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        float rawBPM = pox.getHeartRate(); // Lấy giá trị nhịp tim thô từ thư viện
        int rawSpO2 = pox.getSpO2();       // Lấy giá trị SpO2 thô từ thư viện

        // --- LOGIC "BIỆN PHÁP MẠNH" ĐỂ XỬ LÝ NHIỄU ---
        
        // Điều kiện 1: Nếu đã quá 3 giây mà không thấy nhịp tim nào mới
        bool noFingerTime = (millis() - tsLastBeat > 3000);
        
        // Điều kiện 2: Nếu chỉ số SpO2 quá thấp (Dưới 90% thường là lỗi do hở tay)
        bool lowSpO2 = (rawSpO2 < 90);

        if (noFingerTime || lowSpO2) {
            // Nếu rơi vào 1 trong 2 lỗi trên -> Ép giá trị về 0 để người dùng biết là chưa đo được
            smoothedBPM = 0;
            rawSpO2 = 0; 
        } 
        else {
            // Nếu tín hiệu ổn định:
            if (smoothedBPM == 0) {
                // Nếu đây là lần đầu đo được sau khi mất tín hiệu, lấy luôn giá trị thô
                smoothedBPM = rawBPM;
            } else {
                // Thuật toán EMA (Exponential Moving Average): 
                // Lấy 70% giá trị cũ cộng 30% giá trị mới để con số thay đổi mượt mà
                smoothedBPM = (smoothedBPM * 0.7) + (rawBPM * 0.3);
            }
        }
        // ---------------------------------------------

        // --- CẬP NHẬT GIAO DIỆN MÀN HÌNH OLED ---
        display.clearDisplay(); 
        display.setFont(&FreeSerif9pt7b);
        
        // In tiêu đề
        display.setCursor(10,12); 
        display.print("Pulse Oximeter");     
        
        // In dòng Nhịp tim (BPM)
        display.setCursor(0,35); 
        display.print("HeartR:");
        display.setCursor(62,35);
        display.print((int)smoothedBPM); // Ép kiểu sang số nguyên (int) cho gọn
        display.println(" bpm");
        
        // In dòng Nồng độ Oxy (SpO2)
        display.setCursor(0,59);
        display.print("SpO2  : ");
        display.setCursor(62,59);
        display.print(rawSpO2);
        display.println(" %");
        
        display.display(); // Xuất dữ liệu ra màn hình

        // In dữ liệu ra Serial Monitor để theo dõi trên máy tính (Debug)
        Serial.print("BPM:"); Serial.print(smoothedBPM);
        Serial.print(" SpO2:"); Serial.println(rawSpO2);

        // Cập nhật lại mốc thời gian cuối cùng báo cáo
        tsLastReport = millis();
    }
}
