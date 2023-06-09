// game board is 10 wide by 20 tall, so "pixels" can be 2x2 and still have room to fit 2 players side-by-side
// keep array of arrays to track the game board
//   array of size 10 for each line
//   array items correspond to the color of that pixel (or none)

// pieces will be stored in variables ahead of time
// hard-coded or calculated rotation?
//    here is the official rotations with the spawn orientation on the left: https://static.wikia.nocookie.net/tetrisconcept/images/3/3d/SRS-pieces.png/revision/latest?cb=20060626173148
//    I and O spawn in center, others favor left of center
// piece colors:
//    I: light blue/teal
//    J: dark blue
//    L: orange
//    O: yellow
//    S: green (not dark)
//    T: pink
//    Z: red

// as a piece falls (no input)
//   check to see that 