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
    text::{Baseline, Text},
    Drawable, Pixel,
};
use lazy_static::lazy_static;
use spin::Mutex;

const FONT_WIDTH: usize = FONT_10X20.character_size.width as usize;
const FONT_HEIGHT: usize = FONT_10X20.character_size.height as usize;

lazy_static! {
    pub static ref DISPLAY: Mutex<Option<Display>> = Mutex::new(None);
}

pub struct Display {
    framebuffer: &'static mut [u8],
    info: FrameBufferInfo,
    writer_pos: Position,
    background_color: Color,
}

pub fn init_display(framebuffer: &'static mut FrameBuffer) {
    let mut display = Display::new(framebuffer);
    let color = display.background_color;
    display
        .clear(Rgb888::new(color.red, color.green, color.blue))
        .unwrap();

    DISPLAY.lock().replace(display);
    draw_something();
}

impl Display {
    pub fn new(framebuffer: &'static mut FrameBuffer) -> Display {
        Self {
            info: framebuffer.info(),
            framebuffer: framebuffer.buffer_mut(),
            writer_pos: Position { x: 0, y: 0 },
            background_color: Color {
                red: Rgb888::CSS_DARK_GRAY.r(),
                green: Rgb888::CSS_DARK_GRAY.g(),
                blue: Rgb888::CSS_DARK_GRAY.b(),
            },
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

    fn fill_rect(&mut self, top_left: Point, size: Size, color: Rgb888) {
        for i in top_left.x..(top_left.x + size.width as i32) {
            self.draw_pixel(Point::new(i, top_left.y), color);
        }
        let pixel_bytes = &mut self.framebuffer;
        for i in top_left.y..(top_left.y + size.height as i32 - 1) {
            let src = i as usize * size.width as usize;
            let dest = (i + 1) as usize * size.width as usize;
            pixel_bytes.copy_within(
                (src * self.info.bytes_per_pixel)
                    ..((src + size.width as usize) * self.info.bytes_per_pixel),
                dest * self.info.bytes_per_pixel,
            );
        }
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

    fn clear(&mut self, color: Self::Color) -> Result<(), Self::Error> {
        self.fill_rect(
            Point::new(0, 0),
            Size::new(self.info.stride as u32, self.info.height as u32),
            color,
        );

        self.writer_pos.x = 0;
        self.writer_pos.y = 0;
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
        self.writer_pos.x = 0;
        self.writer_pos.y += FONT_HEIGHT;

        if self.writer_pos.y > (self.info.height - FONT_HEIGHT) {
            self.scroll();
            self.writer_pos.y = self.info.height - FONT_HEIGHT;
        }
    }

    fn scroll(&mut self) {
        let pixel_bytes = &mut self.framebuffer;
        let last_len = self.info.height - FONT_HEIGHT;
        let shift = FONT_HEIGHT - (self.info.height - self.writer_pos.y);

        for i in 0..last_len {
            let src = (i + shift) * self.info.stride;
            let dest = i * self.info.stride;
            pixel_bytes.copy_within(
                (src * self.info.bytes_per_pixel)
                    ..((src + self.info.stride) * self.info.bytes_per_pixel),
                dest * self.info.bytes_per_pixel,
            );
        }

        let bg = self.background_color;
        let top_left = Point::new(0, self.info.height as i32 - FONT_HEIGHT as i32);
        let size = Size::new(self.info.stride as u32, FONT_HEIGHT as u32);
        self.fill_rect(top_left, size, Rgb888::new(bg.red, bg.green, bg.blue));
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
        if c == '\n' || self.writer_pos.x + FONT_WIDTH > self.info.width {
            self.new_line();
            if c == '\n' {
                return Ok(());
            }
        }
        let style = MonoTextStyle::new(&FONT_10X20, Rgb888::YELLOW);
        Text::with_baseline(
            c.encode_utf8(&mut [0; 4]),
            Point::new(self.writer_pos.x as i32, self.writer_pos.y as i32),
            style,
            Baseline::Top,
        )
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
