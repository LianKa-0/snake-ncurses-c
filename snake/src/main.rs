use std::time::{Duration,Instant};
use std::thread::sleep;

const SIZE: usize = 60;

#[derive(Copy, Clone)]
enum Types {
    Wall,
    Fruit,
    Snake,
    Void
}

fn main() {

    /*println!("\u{1b}[31mTest");
    let mut array_2d = init_array();
    let mut beginning = Instant::now();
    let dur = Duration::from_millis(200);

    loop {
        
            draw(&mut array_2d);
            sleep(dur);
            print!("\u{1b}[2J");
            
    
    }*/

    
}


fn iterate_array_2d(array_2d: &mut [[Types; SIZE]; SIZE], f: fn(&mut [[Types; SIZE]; SIZE], y: usize, x: usize)) {
    for y in 0..SIZE {
        for x in 0..SIZE {
            f(array_2d, y, x);
        }
    }
}

fn init_array() -> [[Types; SIZE]; SIZE]{
    let mut array_2d: [[Types; SIZE]; SIZE] = [[Types::Void; SIZE]; SIZE];
    
    iterate_array_2d(&mut array_2d,|a,y,x| {
        if y == 0 || x == 0 || y == SIZE - 1 || x == SIZE -1 {
            a[y][x] = Types::Wall;
        }
    });
    array_2d
    
}

fn draw(array_2d: &mut [[Types; SIZE]; SIZE] ) {
   iterate_array_2d(array_2d, |a,y,x| {
       print!("{}", get_character(&a[y][x]));
       if x == SIZE - 1 {print!("\n")}
   })
}


fn get_character(c: &Types) -> &str {
    match c {
        Types::Wall => "#",
        Types::Fruit => "\u{009E}",
        Types::Snake => "\u{1b}[31md",
        Types::Void => " "
    }
}



