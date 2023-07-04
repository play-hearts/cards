import chai from 'chai'
import chaiAsPromised from 'chai-as-promised'
import {all_ranks, rankOrd, rankName} from './cards.js'
import {all_suits, suitOrd, suitName} from './cards.js'
import {all_seat_names, seatPosition, seatName} from './cards.js'
import {
    kNoCard,
    isSuitName,
    isRankName,
    isCard,
    isNoCard,
    isTrickCard,
    isTrick,
} from './cards.js'

chai.use(chaiAsPromised)
const {expect} = chai

describe('cards_ts', (): void => {
    it('all_ranks and all_suits are correct', async () => {
        expect(all_ranks).to.deep.equal([
            '2',
            '3',
            '4',
            '5',
            '6',
            '7',
            '8',
            '9',
            '10',
            'jack',
            'queen',
            'king',
            'ace',
        ])
        expect(all_suits).to.deep.equal([
            'clubs',
            'diamonds',
            'spades',
            'hearts',
        ])
    })

    it('suitOrd and rankOrd are correct', async () => {
        expect(suitOrd('clubs')).to.equal(0)
        expect(suitOrd('diamonds')).to.equal(1)
        expect(suitOrd('spades')).to.equal(2)
        expect(suitOrd('hearts')).to.equal(3)

        expect(rankOrd('2')).to.equal(0)
        expect(rankOrd('3')).to.equal(1)
        expect(rankOrd('4')).to.equal(2)
        expect(rankOrd('5')).to.equal(3)
        expect(rankOrd('6')).to.equal(4)
        expect(rankOrd('7')).to.equal(5)
        expect(rankOrd('8')).to.equal(6)
        expect(rankOrd('9')).to.equal(7)
        expect(rankOrd('10')).to.equal(8)
        expect(rankOrd('jack')).to.equal(9)
        expect(rankOrd('queen')).to.equal(10)
        expect(rankOrd('king')).to.equal(11)
        expect(rankOrd('ace')).to.equal(12)
    })

    it('rankName and suitName are correct', async () => {
        expect(rankName(0)).to.equal('2')
        expect(rankName(1)).to.equal('3')
        expect(rankName(2)).to.equal('4')
        expect(rankName(3)).to.equal('5')
        expect(rankName(4)).to.equal('6')
        expect(rankName(5)).to.equal('7')
        expect(rankName(6)).to.equal('8')
        expect(rankName(7)).to.equal('9')
        expect(rankName(8)).to.equal('10')
        expect(rankName(9)).to.equal('jack')
        expect(rankName(10)).to.equal('queen')
        expect(rankName(11)).to.equal('king')
        expect(rankName(12)).to.equal('ace')

        expect(suitName(0)).to.equal('clubs')
        expect(suitName(1)).to.equal('diamonds')
        expect(suitName(2)).to.equal('spades')
        expect(suitName(3)).to.equal('hearts')
    })

    it('all_seat_names is correct', async () => {
        expect(all_seat_names).to.deep.equal(['south', 'west', 'north', 'east'])
    })

    it('seatPosition and seatName are correct', async () => {
        expect(seatPosition('south')).to.equal(0)
        expect(seatPosition('west')).to.equal(1)
        expect(seatPosition('north')).to.equal(2)
        expect(seatPosition('east')).to.equal(3)

        expect(seatName(0)).to.equal('south')
        expect(seatName(1)).to.equal('west')
        expect(seatName(2)).to.equal('north')
        expect(seatName(3)).to.equal('east')
    })

    it('isSuitName is correct', async () => {
        expect(isSuitName('clubs')).to.be.true
        expect(isSuitName('diamonds')).to.be.true
        expect(isSuitName('spades')).to.be.true
        expect(isSuitName('hearts')).to.be.true

        expect(isSuitName('club')).to.be.false
        expect(isSuitName('diamond')).to.be.false
        expect(isSuitName('spade')).to.be.false
        expect(isSuitName('heart')).to.be.false

        expect(isSuitName('')).to.be.false
        expect(isSuitName('foo')).to.be.false
        expect(isSuitName('bar')).to.be.false
        expect(isSuitName('baz')).to.be.false
    })

    it('isRankName is correct', async () => {
        expect(isRankName('2')).to.be.true
        expect(isRankName('3')).to.be.true
        expect(isRankName('4')).to.be.true
        expect(isRankName('5')).to.be.true
        expect(isRankName('6')).to.be.true
        expect(isRankName('7')).to.be.true
        expect(isRankName('8')).to.be.true
        expect(isRankName('9')).to.be.true
        expect(isRankName('10')).to.be.true
        expect(isRankName('jack')).to.be.true
        expect(isRankName('queen')).to.be.true
        expect(isRankName('king')).to.be.true
        expect(isRankName('ace')).to.be.true

        expect(isRankName('1')).to.be.false
        expect(isRankName('11')).to.be.false
        expect(isRankName('12')).to.be.false
        expect(isRankName('13')).to.be.false

        expect(isRankName('')).to.be.false
        expect(isRankName('foo')).to.be.false
        expect(isRankName('bar')).to.be.false
        expect(isRankName('baz')).to.be.false
    })

    it('isCard is correct', async () => {
        expect(isCard({rank: '2', suit: 'clubs'})).to.be.true
        expect(isCard({rank: '3', suit: 'clubs'})).to.be.true
        expect(isCard({rank: '4', suit: 'clubs'})).to.be.true
        expect(isCard({rank: '5', suit: 'clubs'})).to.be.true
        expect(isCard({rank: '6', suit: 'clubs'})).to.be.true
        expect(isCard({rank: '7', suit: 'clubs'})).to.be.true
        expect(isCard({rank: '8', suit: 'clubs'})).to.be.true
        expect(isCard({rank: '9', suit: 'clubs'})).to.be.true
        expect(isCard({rank: '10', suit: 'clubs'})).to.be.true
        expect(isCard({rank: 'jack', suit: 'clubs'})).to.be.true
        expect(isCard({rank: 'queen', suit: 'clubs'})).to.be.true
        expect(isCard({rank: 'king', suit: 'clubs'})).to.be.true
        expect(isCard({rank: 'ace', suit: 'clubs'})).to.be.true

        expect(isCard({rank: '2', suit: 'diamonds'})).to.be.true
        expect(isCard({rank: '3', suit: 'diamonds'})).to.be.true
        expect(isCard({rank: '4', suit: 'diamonds'})).to.be.true
        expect(isCard({rank: '5', suit: 'diamonds'})).to.be.true
        expect(isCard({rank: '6', suit: 'diamonds'})).to.be.true
        expect(isCard({rank: '7', suit: 'diamonds'})).to.be.true
        expect(isCard({rank: '8', suit: 'diamonds'})).to.be.true
        expect(isCard({rank: '9', suit: 'diamonds'})).to.be.true
        expect(isCard({rank: '10', suit: 'diamonds'})).to.be.true
        expect(isCard({rank: 'jack', suit: 'diamonds'})).to.be.true
        expect(isCard({rank: 'queen', suit: 'diamonds'})).to.be.true
        expect(isCard({rank: 'king', suit: 'diamonds'})).to.be.true
        expect(isCard({rank: 'ace', suit: 'diamonds'})).to.be.true

        expect(isCard({rank: '2', suit: 'spades'})).to.be.true
        expect(isCard({rank: '3', suit: 'spades'})).to.be.true
        expect(isCard({rank: '4', suit: 'spades'})).to.be.true
        expect(isCard({rank: '5', suit: 'spades'})).to.be.true
        expect(isCard({rank: '6', suit: 'spades'})).to.be.true
        expect(isCard({rank: '7', suit: 'spades'})).to.be.true
        expect(isCard({rank: '8', suit: 'spades'})).to.be.true
        expect(isCard({rank: '9', suit: 'spades'})).to.be.true
        expect(isCard({rank: '10', suit: 'spades'})).to.be.true
        expect(isCard({rank: 'jack', suit: 'spades'})).to.be.true
        expect(isCard({rank: 'queen', suit: 'spades'})).to.be.true
        expect(isCard({rank: 'king', suit: 'spades'})).to.be.true
        expect(isCard({rank: 'ace', suit: 'spades'})).to.be.true

        expect(isCard({rank: '2', suit: 'hearts'})).to.be.true
        expect(isCard({rank: '3', suit: 'hearts'})).to.be.true
        expect(isCard({rank: '4', suit: 'hearts'})).to.be.true
        expect(isCard({rank: '5', suit: 'hearts'})).to.be.true
        expect(isCard({rank: '6', suit: 'hearts'})).to.be.true
        expect(isCard({rank: '7', suit: 'hearts'})).to.be.true
        expect(isCard({rank: '8', suit: 'hearts'})).to.be.true
        expect(isCard({rank: '9', suit: 'hearts'})).to.be.true
        expect(isCard({rank: '10', suit: 'hearts'})).to.be.true
        expect(isCard({rank: 'jack', suit: 'hearts'})).to.be.true
        expect(isCard({rank: 'queen', suit: 'hearts'})).to.be.true
        expect(isCard({rank: 'king', suit: 'hearts'})).to.be.true
        expect(isCard({rank: 'ace', suit: 'hearts'})).to.be.true

        expect(isCard({rank: '1', suit: 'clubs'})).to.be.false
        expect(isCard({rank: '11', suit: 'diamonds'})).to.be.false
        expect(isCard({rank: '', suit: 'spades'})).to.be.false
        expect(isCard({rank: 1, suit: 'hearts'})).to.be.false
        expect(isCard({rank: null, suit: 'clubs'})).to.be.false
        expect(isCard({suit: 'diamonds'})).to.be.false

        expect(isCard({rank: '2', suit: 'foo'})).to.be.false
        expect(isCard({rank: '3', suit: 3})).to.be.false
        expect(isCard({rank: '4', suit: null})).to.be.false
        expect(isCard({rank: '5'})).to.be.false
    })

    it('isNoCard is correct', function () {
        expect(isNoCard({rank: '2', suit: 'clubs'})).to.be.false
        expect(isNoCard({rank: '3', suit: 'clubs'})).to.be.false
        expect(isNoCard({rank: '', suit: ''})).to.be.true
    })

    it('isTrickCard is correct', function () {
        expect(isTrickCard({rank: '', suit: ''})).to.be.true
        expect(isTrickCard({rank: '2', suit: 'spades'})).to.be.true
        expect(isTrickCard({rank: '3', suit: 'spades'})).to.be.true
        expect(isTrickCard({rank: '4', suit: 'spades'})).to.be.true
        expect(isTrickCard({rank: '5', suit: 'spades'})).to.be.true
        expect(isTrickCard({rank: '6', suit: 'spades'})).to.be.true
        expect(isTrickCard({rank: '7', suit: 'spades'})).to.be.true
        expect(isTrickCard({rank: '8', suit: 'spades'})).to.be.true
        expect(isTrickCard({rank: '9', suit: 'spades'})).to.be.true
        expect(isTrickCard({rank: '10', suit: 'spades'})).to.be.true
        expect(isTrickCard({rank: 'jack', suit: 'spades'})).to.be.true
        expect(isTrickCard({rank: 'queen', suit: 'spades'})).to.be.true
        expect(isTrickCard({rank: 'king', suit: 'spades'})).to.be.true
        expect(isTrickCard({rank: 'ace', suit: 'spades'})).to.be.true

        expect(isTrickCard({rank: '2', suit: ''})).to.be.false
        expect(isTrickCard({rank: '', suit: 'spades'})).to.be.false
    })

    it('itTrick is correct', function () {
        expect(isTrick([kNoCard, kNoCard, kNoCard, kNoCard])).to.be.true
        expect(
            isTrick([kNoCard, kNoCard, kNoCard, {rank: '2', suit: 'spades'}])
        ).to.be.true
        expect(
            isTrick([
                kNoCard,
                kNoCard,
                {rank: '2', suit: 'spades'},
                {rank: '2', suit: 'spades'},
            ])
        ).to.be.true
        expect(
            isTrick([
                kNoCard,
                {rank: '2', suit: 'spades'},
                {rank: '2', suit: 'spades'},
                {rank: '2', suit: 'spades'},
            ])
        ).to.be.true
        expect(
            isTrick([
                {rank: '2', suit: 'spades'},
                {rank: '2', suit: 'spades'},
                {rank: '2', suit: 'spades'},
                {rank: '2', suit: 'spades'},
            ])
        ).to.be.true
        expect(
            isTrick([kNoCard, {rank: '2', suit: 'spades'}, kNoCard, kNoCard])
        ).to.be.true

        expect(isTrick([])).to.be.false
        expect(isTrick([kNoCard])).to.be.false
        expect(isTrick([kNoCard, kNoCard])).to.be.false
        expect(isTrick([kNoCard, kNoCard, kNoCard])).to.be.false
    })
})
