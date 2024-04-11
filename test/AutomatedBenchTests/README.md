# NodeJS Bench Test Automation

To run: `make bench_test`

- To run manually
   - `nvm i`
   - `npm i`
   - `npm run bench_test`

## Folders

#### `test`

- Any `js` file in this folder is ran during a bench test
- This folder is ignored by git and is meant for scratch work
- If you feel there needs to be more boiler plate in `bench.js`, feel free to commit it directly

#### `rockhopper`

- This folder is not ignored by git
- Place `js` files you think will be helpful for future bench tests, like getting the board into certain states

#### `setup`

- For setup stuffs

## FAQ

- The board will reset before a `describe` is run
   - You can nest describes, which will still result in a reset
   - You can also do `await board.reset()`

- You may use a global function called `beforeEachDescribe` to run something before every `describe`
   - Since you will probably only have one file, it is probably easier to do `beforeAll` directly inside the `describe`
   - You can also nest a `describe` inside another, and the inner `describe` can have a `beforeAll`, allowing for selective composability
