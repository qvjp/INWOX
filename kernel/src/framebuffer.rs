use bootloader_api::info::{FrameBuffer, FrameBufferInfo, PixelFormat};
use core::fmt;
use embedded_graphics::{
    draw_target::DrawTarget,
    geometry,
    geometry::Point,
    mono_font::iso_8859_16::FONT_10X20,
    mono_font::MonoTextStyle,
    pixelcolor::{Rgb888, RgbColor},
    prelude::*,
    primitives::{Polyline, PrimitiveStyle},
    text::Text,
    Drawable, Pixel,
};
use lazy_static::lazy_static;
use spin::Mutex;

const FONT_WIDTH: i32 = FONT_10X20.character_size.width as i32;
const FONT_HEIGHT: i32 = FONT_10X20.character_size.height as i32;

lazy_static! {
    pub static ref DISPLAY: Mutex<Option<Display>> = Mutex::new(None);
}

pub struct Display {
    framebuffer: &'static mut [u8],
    info: FrameBufferInfo,
    writer_pos: Point,
}

pub fn init_display(framebuffer: &'static mut FrameBuffer) {
    let mut display = Display::new(framebuffer);
    display.clear(Rgb888::BLACK).unwrap();

    DISPLAY.lock().replace(display);
    draw_something();
}

impl Display {
    pub fn new(framebuffer: &'static mut FrameBuffer) -> Display {
        Self {
            info: framebuffer.info(),
            framebuffer: framebuffer.buffer_mut(),
            writer_pos: Point::new(0, FONT_HEIGHT),
        }
    }

    fn draw_pixel(&mut self, coordinates: Point, color: Rgb888) {
        let position = match (coordinates.x.try_into(), coordinates.y.try_into()) {
            (Ok(x), Ok(y)) if x < self.info.width && y < self.info.height => Position { x, y },
            _ => return,
        };
        let color = Color {
            red: color.r(),
            green: color.g(),
            blue: color.b(),
        };
        set_pixel_in(self.framebuffer, self.info, position, color);
    }
}

impl DrawTarget for Display {
    type Color = Rgb888;

    type Error = core::convert::Infallible;

    fn draw_iter<I>(&mut self, pixels: I) -> Result<(), Self::Error>
    where
        I: IntoIterator<Item=Pixel<Self::Color>>,
    {
        for Pixel(coordinates, color) in pixels.into_iter() {
            self.draw_pixel(coordinates, color);
        }
        Ok(())
    }
}

impl geometry::OriginDimensions for Display {
    fn size(&self) -> geometry::Size {
        geometry::Size::new(
            self.info.width.try_into().unwrap(),
            self.info.height.try_into().unwrap(),
        )
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct Position {
    pub x: usize,
    pub y: usize,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct Color {
    pub red: u8,
    pub green: u8,
    pub blue: u8,
}

pub fn set_pixel_in(
    framebuffer: &mut [u8],
    info: FrameBufferInfo,
    position: Position,
    color: Color,
) {
    let byte_offset = {
        let line_offset = position.y * info.stride;
        let pixel_offset = line_offset + position.x;
        pixel_offset * info.bytes_per_pixel
    };

    let pixel_bytes = &mut framebuffer[byte_offset..];
    match info.pixel_format {
        PixelFormat::Rgb => {
            pixel_bytes[0] = color.red;
            pixel_bytes[1] = color.green;
            pixel_bytes[2] = color.blue;
        }
        PixelFormat::Bgr => {
            pixel_bytes[0] = color.blue;
            pixel_bytes[1] = color.green;
            pixel_bytes[2] = color.red;
        }
        PixelFormat::U8 => {
            // use a simple average-based grayscale transform
            let gray = color.red / 3 + color.green / 3 + color.blue / 3;
            pixel_bytes[0] = gray;
        }
        other => panic!("unknown pixel format {other:?}"),
    }
}

impl Display {
    fn new_line(&mut self) {
        self.writer_pos.y += FONT_HEIGHT;
        self.writer_pos.x = 0;

        if self.writer_pos.y >= self.info.height as i32 {
            self.writer_pos.y -= FONT_HEIGHT;
            self.scroll();
        }
    }

    fn scroll(&mut self) {
        let pixel_bytes = &mut self.framebuffer;
        for i in 0..self.info.width * (self.info.height - FONT_HEIGHT as usize) {
            let a = i * self.info.bytes_per_pixel;
            let b = (i + self.info.width * FONT_HEIGHT as usize) * self.info.bytes_per_pixel;
            for i in 0..self.info.bytes_per_pixel {
                pixel_bytes[a + i] = pixel_bytes[b + i];
            }
        }
        for i in self.info.width * (self.info.height - FONT_HEIGHT as usize) + 1
            ..self.info.width * self.info.height
        {
            pixel_bytes[i * self.info.bytes_per_pixel] = Rgb888::BLACK.r();
            pixel_bytes[i * self.info.bytes_per_pixel + 1] = Rgb888::BLACK.g();
            pixel_bytes[i * self.info.bytes_per_pixel + 2] = Rgb888::BLACK.b();
        }
    }
}

impl fmt::Write for Display {
    fn write_str(&mut self, s: &str) -> fmt::Result {
        for c in s.chars() {
            self.write_char(c)?
        }
        Ok(())
    }

    fn write_char(&mut self, c: char) -> fmt::Result {
        if c == '\n' || self.writer_pos.x + FONT_WIDTH > self.info.width as i32 {
            self.new_line();
            if c == '\n' {
                return Ok(());
            }
        }
        let style = MonoTextStyle::new(&FONT_10X20, Rgb888::YELLOW);
        Text::new(c.encode_utf8(&mut [0; 4]), self.writer_pos, style)
            .draw(self)
            .unwrap();
        self.writer_pos.x += FONT_WIDTH;
        Ok(())
    }
}

fn draw_something() {
    if let Some(display) = &mut *DISPLAY.lock() {
        let x_off = (display.info.width - 350) as i32;
        let y_off = 100;
        let points: [Point; 10] = [
            Point::new(10 + x_off, 64 + y_off),
            Point::new(50 + x_off, 64 + y_off),
            Point::new(60 + x_off, 44 + y_off),
            Point::new(70 + x_off, 64 + y_off),
            Point::new(80 + x_off, 64 + y_off),
            Point::new(90 + x_off, 74 + y_off),
            Point::new(100 + x_off, 10 + y_off),
            Point::new(110 + x_off, 84 + y_off),
            Point::new(120 + x_off, 64 + y_off),
            Point::new(300 + x_off, 64 + y_off),
        ];

        let line_style = PrimitiveStyle::with_stroke(Rgb888::GREEN, 3);

        Polyline::new(&points)
            .into_styled(line_style)
            .draw(display)
            .unwrap();
    }
}

#[macro_export]
macro_rules! print {
    ($($arg:tt)*) => ($crate::framebuffer::_print(format_args!($($arg)*)));
}

#[macro_export]
macro_rules! println {
    () => ($crate::print!("\n"));
    ($($arg:tt)*) => ($crate::print!("{}\n", format_args!($($arg)*)));
}

#[doc(hidden)]
pub fn _print(args: fmt::Arguments) {
    use core::fmt::Write;
    use x86_64::instructions::interrupts;
    interrupts::without_interrupts(|| {
        if let Some(display) = &mut *DISPLAY.lock() {
            display.write_fmt(args).unwrap();
        }
    });
}
