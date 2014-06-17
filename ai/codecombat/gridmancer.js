// Fill the empty space with the minimum number of rectangles.
// (Rectangles should not overlap each other or walls.)
// The grid size is 1 meter, but the smallest wall/floor tile is 4 meters.
// Check the blue guide button at the top for more info.
// Make sure to sign up on the home page to save your code.

var _grid = this.getNavGrid().grid;
var tileSize = 4;
var minY = 0, maxY = 0, minX = 0, maxX = 0;

var grid = [];

var lastLen = 0;
var x = 0, y = 0;
var xi = 0, yi = 0;

for (y = 0; y + tileSize < _grid.length; y += tileSize)
{
    grid.push([]);
    for(x = 0; x + tileSize < _grid[0].length; x += tileSize)
    {
        xi = x / tileSize;
        yi = y / tileSize;
        grid[yi].push(_grid[y][x].length ? 0 : 1);
        if (grid[yi][xi])
        {
            for (var xx = xi-1; xx >= 0; xx -= 1)
            {
                if (grid[yi][xx] !== 0) grid[yi][xx] += 1;
                else break;
            }
        }
    } 
}

//this.addRect(8, 6, 16, 12);

for (x = 0; x < grid[0].length; x += 1)
{
    for(y = 0; y < grid.length; y += 1)
    {
        var dataLen = grid[y][x];
        if (lastLen <= 0)
        {
            if (dataLen > 0)
            {
                lastLen = dataLen;
                minX = x;
                minY = y;
                //this.say(minX + " " + minY);
            }
        }
        else
        {
            if (dataLen < lastLen)
            {
                maxX = x + lastLen;
                maxY = y;

                if (maxX == minX) maxX += 1; 
                if (maxY == minY) maxY += 1; 
                //this.say(minX + " " + minY + " " + maxX + " " + maxY);

                for (xi = minX; xi < maxX; xi += 1)
                {
                    for (yi = minY; yi < maxY; yi += 1)
                    {
                        grid[yi][xi] = 0;
                    }
                }

                minX *= tileSize;
                minY *= tileSize;
                maxX *= tileSize;
                maxY *= tileSize;

                this.addRect(
                    (maxX - minX)/2 + minX,
                    (maxY - minY)/2 + minY,
                    maxX - minX,
                    maxY - minY);

                //this.say(minX + " " + minY + " " + maxX + " " + maxY);
                this.wait();

                minX = 0;
                minY = 0;
                maxX = 0;
                maxY = 0;
                lastLen = 0;

                if (dataLen > 0)
                {
                    lastLen = dataLen;
                    minX = x;
                    minY = y;
                }
            }
        }
    }
}