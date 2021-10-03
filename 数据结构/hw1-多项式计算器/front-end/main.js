function getInput(index) {
    let s = document.getElementById(`p${index}`).value;
    if (s == '0' || s == '') {
        return {
            leng: 0,
            s: ''
        }
    }
    s = s.replace(/{|}/g, '');  // 便于处理 tex
    let array = s.replace(/-/g, '+-').replace(/\^\+/g, '^').replace(/^\+/, '').split('+'); // 加号分割每一项
    let ans = ''; const length = array.length * 2;
    array.forEach((item) => {
        //对每一个 item, 希望格式为: 'num1 num2 '
        if (item.indexOf('x') == -1) {           // 常数项
            ans += `${item} 0 `;
        } else {
            // 先考虑要不要补系数
            item = item.replace(/^-?x/, (s) => `${s.slice(0, s.length - 1)}1x`);
            if (item.indexOf('x') == item.length - 1) { // 一次项
                ans += `${item.slice(0, item.length - 1)} 1 `;
            } else {
                item = item.replace(/x\^/, ' ');
                ans += item + ' ';
            }
        }
    })
    return {
        leng: length,
        s: ans
    }
}

const getOutput = (data) => {
    if (data === '0') return `$${data}$`;
    let array = data.split(' ')
    const terms = (array.length - 1) / 2;
    let res = ''
    for (let i = 0; i < terms; i++) {
        // array[2 * i] 是系数, array[2 * i + 1] 是指数
        if (array[2 * i][0] != '-') {           // 系数区分正负
            array[2 * i] = '+' + array[2 * i];
        }
        if (array[2 * i + 1] == '0') {
            res += array[2 * i];
            continue;
        }
        if (array[2 * i + 1] == '1') {
            if (array[2 * i] == '+1' || array[2 * i] == '-1') {
                res += array[2 * i][0] + 'x';
                continue;
            }
            res += array[2 * i] + 'x';
            continue;
        }
        if (array[2 * i] == '+1' || array[2 * i] == '-1') {
            res += array[2 * i][0] + `x^{${array[2 * i + 1]}}`;
            continue;
        }
        res += array[2 * i] + `x^{${array[2 * i + 1]}}`;
        continue;
    }
    return `$${res[0] == '-' ? res : res.slice(1)}$`;
}

function add() {
    const res1 = getInput(1), res2 = getInput(2);
    fetch(`http://8.142.17.250/testfold/poly.php?op=0&p1=${encodeURIComponent(res1.s)}&p2=${encodeURIComponent(res2.s)}&count1=${res1.leng}&count2=${res2.leng}`)
        .then(response => response.text())
        .then(data => {
            const output = getOutput(data);
            document.getElementById('res1').innerHTML = `${output}<pre>可复制此处：${output.slice(1,output.length-1)}</pre>`;
            document.getElementById('res1').style.display = 'block';
            MathJax.typeset();
        })
}

function minus() {
    const res1 = getInput(1), res2 = getInput(2);
    fetch(`http://8.142.17.250/testfold/poly.php?op=1&p1=${encodeURIComponent(res1.s)}&p2=${encodeURIComponent(res2.s)}&count1=${res1.leng}&count2=${res2.leng}`)
        .then(response => response.text())
        .then(data => {
            const output = getOutput(data);
            document.getElementById('res1').innerHTML = `${output}<pre>可复制此处：${output.slice(1,output.length-1)}</pre>`;
            document.getElementById('res1').style.display = 'block';
            MathJax.typeset();
        })
}

function multi() {
    const res1 = getInput(1), res2 = getInput(2);
    fetch(`http://8.142.17.250/testfold/poly.php?op=2&p1=${encodeURIComponent(res1.s)}&p2=${encodeURIComponent(res2.s)}&count1=${res1.leng}&count2=${res2.leng}`)
        .then(response => response.text())
        .then(data => {
            const output = getOutput(data);
            document.getElementById('res1').innerHTML = `${output}<pre>可复制此处：${output.slice(1,output.length-1)}</pre>`;
            document.getElementById('res1').style.display = 'block';
            MathJax.typeset();
        })
}

function getValue() {
    const r = getInput(3);
    const x = document.getElementById('x').value;
    if (x == '') {
        alert('尚未输入在哪一点求值');
        return;
    }
    fetch(`http://8.142.17.250/testfold/poly.php?op=3&p3=${encodeURIComponent(r.s)}&count3=${r.leng}&value=${x}`)
        .then(response => response.text())
        .then(data => {
            document.getElementById('res2').innerHTML = data;
            document.getElementById('res2').style.display = 'block';
            MathJax.typeset();
        })
}

function diff() {
    const r = getInput(3);
    fetch(`http://8.142.17.250/testfold/poly.php?op=4&p3=${encodeURIComponent(r.s)}&count3=${r.leng}`)
        .then(response => response.text())
        .then(data => {
            const output = getOutput(data);
            document.getElementById('res2').innerHTML = `${output}<pre>可复制此处：${output.slice(1,output.length-1)}</pre>`;
            document.getElementById('res2').style.display = 'block'
            MathJax.typeset();
        })
}

function divide() {
    const res1 = getInput(1), res2 = getInput(2);
    if (res2.leng === 0 || res2.s === '0 0 ') {
        alert('除数不能为空或 0 ，请检查输入');
        return;
    }
    fetch(`http://8.142.17.250/testfold/poly.php?op=5&p1=${encodeURIComponent(res1.s)}&p2=${encodeURIComponent(res2.s)}&count1=${res1.leng}&count2=${res2.leng}`)
        .then(response => response.text())
        .then(data => {
            const ans = data.split('\n');
            // ans[0]是商，ans[1]是余数
            const quotient = getOutput(ans[0]), remainder = getOutput(ans[1]);
            document.getElementById('res1').innerHTML = `商：${quotient}<pre>可复制此处：${quotient.slice(1,quotient.length-1)}</pre>余数：${remainder}<pre>可复制此处：${remainder.slice(1,remainder.length-1)}</pre>`;
            document.getElementById('res1').style.display = 'block';
            MathJax.typeset();
        })
}