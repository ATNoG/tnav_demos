class TSM {
  constructor(w, h, min, max, p) {
    this.w = w;
    this.h = h;
    this.min = min;
    this.max = max;
    this.p = p;
    this.ts0 = 0;
    this.matrix = [];
    for(var i=0; i<w; i++) {
      this.matrix[i] = new Array(h);
      for(var j=0; j<h; j++) {
        this.matrix[i][j] = new Array(h);
      }
    }
  }

  get width() {
    return this.w;
  }

  get height() {
    return this.h;
  }


  computeR(value) {
    var dp = value > this.max ? this.max : value;
    var dp = dp < this.min ? this.min : dp;
    return Math.floor(dp/((this.max-this.min)/this.h))-1;
  }

  computeC(ts) {
    var dt = ts-Math.floor(ts/this.p)*this.p;
    return Math.floor(dt/this.w)-1;
  }

  // does not use ts1 right now
  // it may overflow
  addPoint(ts0, v0, ts1, v1) {
    var r = computeR(v0);
    var c = computeC(ts0);
    var w = computeR(v1);
    this.matrix[r][c][w] = this.matrix[r][c][w]+1.0;
  }
  
  

  toString() {
    var line=new Array(this.h);
    for(var i=0; i<this.h; i++) {
      var pr=this.matrix[this.w-1][i];
      for(var j=0; j<this.h; j++) {
        
      }
    }

    for()
    return "Test toString...";
  }
}
