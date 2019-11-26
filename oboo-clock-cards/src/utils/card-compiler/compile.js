const UglifyJS = require("uglify-js")
const fs = require('fs');

const srcDir = './src/'
const buildDir = './build/'
const cCodeTemplatePath = './code_template.c'

var code = {}

fs.readdirSync(srcDir).forEach(file => {
  code[file] = fs.readFileSync( srcDir + file, "utf8")
})

var cCodeTemplate = fs.readFileSync( cCodeTemplatePath, "utf8")

var options = {
    mangle: true,
    compress: {
        // hoist_funs: true,
        passes: 2
    },
};

var miniCode = UglifyJS.minify(code, options).code

// excape quotes
miniCode = miniCode.replace(/"/g, '\\\"')
// fs.writeFileSync(buildDir+"app.js", miniCode, "utf8")
var cCode = cCodeTemplate.replace('{{js}}',miniCode)
console.log(cCode)
