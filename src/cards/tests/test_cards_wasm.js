// test_cards_jswasm.js

const [exe, script, wasmjs, ...args] = process.argv
const factory = require(wasmjs);
const assert = require('assert');

function logCardSet(instance, cardSet) {
    const cardVec = cardSet.asCardVector();
    for (let i=0; i<cardVec.size(); i++)
    {
        let name = instance.nameOfCard(cardVec.get(i));
        assert.ok(name);
        // console.log(name);
    }
    cardVec.delete()
}

async function Card_test(instance) {
    const card = new instance.Card(37);

    const cardName = instance.nameOfCard(card);
    console.log("Card name:", cardName);

    card.delete()
}

async function Deal_test(instance) {
    const deal = new instance.Deal("");

    const hand = deal.dealFor(0);
    logCardSet(instance, hand);

    hand.delete()
    deal.delete()
}

async function Deal_timing_test(instance) {
    const start = Date.now();
    const iters = 1000;
    for (let i=0; i<iters; ++i)
    {
        const deal = new instance.Deal("");
        deal.delete()
    }
    const end = Date.now();
    const millisPerDeal = (end - start) / iters;
    console.log("ms per deal:", millisPerDeal);
}

async function CardSet_test(instance) {
    const cards = new instance.CardSet();
    assert.strictEqual(cards.size(), 0);
    cards.delete()

    const deal = new instance.Deal("");
    const hand = deal.dealFor(0);
    assert.strictEqual(hand.size(), 13);

    console.log("Hand: ", instance.to_string(hand));

    const pass = instance.chooseThreeAtRandom(hand);
    assert.strictEqual(pass.size(), 3);
    console.log("pass: ", instance.to_string(pass));

    pass.delete()
    hand.delete()
    deal.delete()
}

async function run() {
    const instance = await factory()
    console.log(`Succesfully loaded ${wasmjs}'`)

    await Card_test(instance);
    await Deal_test(instance);
    await Deal_timing_test(instance);
    await CardSet_test(instance);
}

run().then(() => console.log("Goodbye"))
